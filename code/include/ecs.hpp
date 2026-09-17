#pragma once

#include <vector>
#include <iostream>
#include <typetraits.hpp>
#include <slotmap.hpp>

namespace HeatStroke
{

    template<typename CMPLIST>
    struct cmp_traits : MP::type_traits<CMPLIST>
    {

    };

    template<typename TAGLIST>
    struct tag_traits : MP::type_traits<TAGLIST>
    {

    };

    /*
     * EntityManager will provide a way to create and manage entities and their components.
     */
    template<typename CMPS, typename TAGS>
    class EntityManager
    {
        public:
            using cmps = cmp_traits<CMPS>;
            using tags = tag_traits<TAGS>;
            // FW of Entity
            struct Entity;
            
            /*
             * Creates a new entity and returns a reference to it.
             */
            Entity& createEntity();

            /**
             * Adds an existing entity to the manager.
             */
            void addEntity(Entity& entity);

            /**
             * Removes an entity from the manager.
             */
            void removeEntity(std::size_t entityId);

            /*
             * Adds a component to the specified entity.
             */
            template<typename CMP>
            CMP& addComponent(Entity& entity, CMP&& component);

            /*
             * Removes a component from the specified entity.
             */
            template<typename CMP>
            void removeComponent(Entity& entity);

            /*
             * Entity struct will represent an entity in the ECS system with a unique ID and a mask of their components.
             */
            struct Entity
            {
                inline static std::size_t ID_COUNTER = 0;
                
                Entity() = default;
                
                std::size_t getId() const;
                
                // Entity data members
                private:

                    std::size_t id{++ID_COUNTER};

                
            };
            
            EntityManager();
            // Deleted functions
            EntityManager(const EntityManager& rhs) = delete;
            EntityManager& operator=(const EntityManager& rhs) = delete;
            private:
                std::vector<Entity> entities;
    };
        

    template<typename CMPS, typename TAGS>
    EntityManager<CMPS, TAGS>::Entity& EntityManager<CMPS, TAGS>::createEntity()
    {
        return entities.emplace_back();
    }

    template<typename CMPS, typename TAGS>
    void EntityManager<CMPS, TAGS>::addEntity(Entity& entity)
    {
        entities.push_back(entity);
    }

    template<typename CMPS, typename TAGS>
    void EntityManager<CMPS, TAGS>::removeEntity(std::size_t entityId)
    {
        for(std::size_t i = 0; i < entities.size(); ++i)
        {
            if(entities[i].getId() == entityId)
            {
                // Here we must remove all entity components, but for now we will just remove the entity from the list.
                entities[i] = entities.back();
                entities.pop_back();
                std::cout << "Entities size: " << entities.size() << std::endl;
                return;
            }
        }
    }

    template<typename CMPS, typename TAGS>
    std::size_t EntityManager<CMPS, TAGS>::Entity::getId() const
    {
        return id;
    }

    template<typename CMPS, typename TAGS>
    EntityManager<CMPS, TAGS>::EntityManager()
    {
        entities.reserve(1000);
    }
    
} // namespace HeatStroke