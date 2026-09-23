#pragma once

#include <vector>
#include <iostream>
#include <componentstorage.hpp>

namespace HeatStroke
{
    /*
     * EntityManager will provide a way to create and manage entities and their components.
     */
    template<typename CMPS, typename TAGS, std::size_t Capacity = 1000>
    class EntityManager
    {
        public:
            using cmp_storage_t = ComponentStorage<CMPS,TAGS,Capacity>;
            using cmpinfo = ComponentStorage<CMPS,TAGS,Capacity>::cmpinfo;
            using taginfo = ComponentStorage<CMPS,TAGS,Capacity>::taginfo;
            template<typename T>
            using cmp_key = HeatStroke::SlotMap<T,Capacity>::key_type;
            template<typename CMP>
            using Slotmap_t = HeatStroke::SlotMap<CMP,Capacity>;
            
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
             * Entity struct will represent an entity in the ECS system with a unique ID and a mask of their components.
             */
            struct Entity
            {
                using key_to_typlist = MP::for_all_insert_template_t<cmp_key, CMPS>;
                using Key_Storage_t = MP::replace_t<std::tuple, key_to_typlist>;
                
                Entity() = default;
                
                std::size_t getId() const;

                template<typename CMP>
                bool hasComponent() const noexcept
                {
                    return (cmpinfo::template mask<CMP>() & cmp_mask);
                }

                template<typename CMP>
                void addComponent(cmp_key<CMP> key)
                {
                    cmp_mask |= cmpinfo::template mask<CMP>();

                    std::get<cmp_key<CMP> >(keys) = key;
                }

                template<typename CMP>
                void removeComponent()
                {
                    cmp_mask ^= cmpinfo::template mask<CMP>();
                }

                template<typename CMP>
                cmp_key<CMP> getComponentKey() const
                {
                    runTimeAssertion(hasComponent<CMP>());
                    return std::get<cmpinfo::template id<CMP>()>(keys);
                }
                
                // Entity data members
                private:
                    cmpinfo::mask_type cmp_mask {};
                    taginfo::mask_type tag_mask {};
                    std::size_t id{++ID_COUNTER};
                    Key_Storage_t keys {};

                    inline static std::size_t ID_COUNTER {0};


                
            };

            template<typename CMP>
            CMP& addComponent(Entity& e, CMP&& cmp) noexcept
            {
                Slotmap_t<CMP>& storage = components_.template getStorage<CMP>();
                cmp_key<CMP> key;
                
                if(e.template hasComponent<CMP>())
                {
                    warning(); // Should not add a component that the Entity already have
                    key = e.template getComponentKey<CMP>();
                    return storage[key];
                }
                
                // Add component to the slotmap and 
                key = storage.push_back(cmp);
                //Add key to the Entity
                e.template addComponent<CMP>(key);

                return storage[key];
            }

            EntityManager();
            // Deleted functions
            EntityManager(const EntityManager& rhs) = delete;
            EntityManager& operator=(const EntityManager& rhs) = delete;
            private:
                std::vector<Entity> entities_{};
                cmp_storage_t components_{};
    };
        

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    EntityManager<CMPS, TAGS, Capacity>::Entity& EntityManager<CMPS, TAGS, Capacity>::createEntity()
    {
        return entities_.emplace_back();
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    void EntityManager<CMPS, TAGS, Capacity>::addEntity(Entity& entity)
    {
        entities_.push_back(entity);
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    void EntityManager<CMPS, TAGS, Capacity>::removeEntity(std::size_t entityId)
    {
        for(std::size_t i = 0; i < entities_.size(); ++i)
        {
            if(entities_[i].getId() == entityId)
            {
                // Here we must remove all entity components, but for now we will just remove the entity from the list.
                entities_[i] = entities_.back();
                entities_.pop_back();
                std::cout << "Entities size: " << entities_.size() << std::endl;
                return;
            }
        }
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    std::size_t EntityManager<CMPS, TAGS, Capacity>::Entity::getId() const
    {
        return id;
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    EntityManager<CMPS, TAGS, Capacity>::EntityManager()
    {
        entities_.reserve(1000);
    }
    
} // namespace HeatStroke