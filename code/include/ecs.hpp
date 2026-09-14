#pragma once

#include <vector>
#include <iostream>
namespace HeatStroke
{
    /*
     * EntityManager will provide a way to create and manage entities and their components.
     */
    template<std::size_t MAX_CMPS = 1000>
    class EntityManager
    {
        public:
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
        

    template<std::size_t MAX_CMPS>
    EntityManager<MAX_CMPS>::Entity& EntityManager<MAX_CMPS>::createEntity()
    {
        return entities.emplace_back();
    }

    template<std::size_t MAX_CMPS>
    void EntityManager<MAX_CMPS>::addEntity(Entity& entity)
    {
        entities.push_back(entity);
    }

    template<std::size_t MAX_CMPS>
    void EntityManager<MAX_CMPS>::removeEntity(std::size_t entityId)
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

    template<std::size_t MAX_CMPS>
    std::size_t EntityManager<MAX_CMPS>::Entity::getId() const
    {
        return id;
    }

    template<std::size_t MAX_CMPS>
    EntityManager<MAX_CMPS>::EntityManager()
    {
        entities.reserve(MAX_CMPS);
    }
    
} // namespace HeatStroke