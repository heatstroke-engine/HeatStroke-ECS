#pragma once

#include <vector>
#include <iostream>
#include <type_traits>
#include <utility>
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
            
            /*
             * Entity struct will represent an entity in the ECS system with a unique ID and a mask of their components.
             */
            struct Entity
            {
                friend class EntityManager;

                using key_to_typlist = MP::for_all_insert_template_t<cmp_key, CMPS>;
                using Key_Storage_t = MP::replace_t<std::tuple, key_to_typlist>;
                
                Entity() = default;
                
                std::size_t getId() const;

                template<typename CMP>
                bool hasComponent() const noexcept
                {
                    return (cmpinfo::template mask<CMP>() & cmp_mask);
                }

                template<typename TAG>
                bool hasTag() const noexcept
                {
                    return (taginfo::template mask<TAG>() & tag_mask);
                }

                template<typename CMP>
                cmp_key<CMP> getComponentKey() const
                {
                    return std::get<cmpinfo::template id<CMP>()>(keys);
                }
                
                private:
                    template<typename CMP>
                    void addComponent(cmp_key<CMP> key)
                    {
                        cmp_mask |= cmpinfo::template mask<CMP>();

                        std::get<cmp_key<CMP> >(keys) = key;
                    }

                    template<typename CMP>
                    void removeComponent()
                    {
                        cmp_mask &= ~cmpinfo::template mask<CMP>();
                    }

                    template<typename TAG>
                    void addTag()
                    {
                        tag_mask |= taginfo::template mask<TAG>();
                    }

                    template<typename TAG>
                    void removeTag()
                    {
                        tag_mask &= ~taginfo::template mask<TAG>();
                    }
                    
                    cmpinfo::mask_type cmp_mask {};
                    taginfo::mask_type tag_mask {};
                    std::size_t id{++ID_COUNTER};
                    Key_Storage_t keys {};

                    inline static std::size_t ID_COUNTER {0};

            };

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

            /**
             * Add the given component to the given Entity.
             */
            template<typename CMP>
            std::remove_cvref_t<CMP>& addComponent(Entity& e, CMP&& cmp) noexcept;

            /**
             * Add the given components to the given Entity.
             */
            template<typename... CMPs>
            void addComponents(Entity& e, CMPs&&... cmp) noexcept;

            /**
             * Deletes the given component of the given Entity.
             */
            template<typename CMP>
            void removeComponent(Entity& e) noexcept;

            /**
             * Deletes the given components of the given Entity.
             */
            template<typename... CMPs>
            void removeComponents(Entity& e) noexcept;   
            
            template<typename TAG>
            void addTag(Entity& e) noexcept;

            template<typename... TAGs>
            void addTags(Entity& e) noexcept;

            template<typename TAG>
            void removeTag(Entity& e) noexcept;

            template<typename... TAGs>
            void removeTags(Entity& e) noexcept;

            template<typename CMP>
            CMP& getComponent(const Entity& e) noexcept;

            template<typename CMP>
            const CMP& getComponent(const Entity& e) const noexcept;

            EntityManager();
            // Deleted functions
            EntityManager(const EntityManager& rhs) = delete;
            EntityManager& operator=(const EntityManager& rhs) = delete;
            private:
                template<typename CMP>
                void removeIfPresent(Entity& e) noexcept
                {
                    if (e.template hasComponent<CMP>()) 
                    {
                            removeComponent<CMP>(e);
                    }
                }


                template<std::size_t... I>
                void removeEntityimp(Entity& e, std::index_sequence<I...>)noexcept
                {
                   (removeIfPresent<typename cmpinfo::template at<I> >(e), ...);
                }


                std::vector<Entity> entities_{};
                cmp_storage_t components_{};
    };
        

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    inline EntityManager<CMPS, TAGS, Capacity>::Entity& EntityManager<CMPS, TAGS, Capacity>::createEntity()
    {
        return entities_.emplace_back();
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    inline void EntityManager<CMPS, TAGS, Capacity>::addEntity(Entity& entity)
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

                // Remove Entity components
                removeEntityimp(entities_[i], std::make_index_sequence<cmpinfo::list_size>{});
                // Remove Entity
                if(entityId != entities_.back().getId())
                {
                    entities_[i] = entities_.back();
                }
                entities_.pop_back();
                break;
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


    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename... CMPs>
    void EntityManager<CMPS, TAGS, Capacity>::addComponents(Entity& e, CMPs&&... cmp) noexcept
    {
        (addComponent(e, std::forward<CMPs>(cmp)), ...);
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename CMP>
    std::remove_cvref_t<CMP>& EntityManager<CMPS, TAGS, Capacity>::addComponent(Entity& e, CMP&& cmp) noexcept
    {
        using T = std::remove_cvref_t<CMP>;

        Slotmap_t<T>& storage = components_.template getStorage<T>();
        cmp_key<T> key;

        debugAssertion(!e.template hasComponent<T>(), "Trying to add a component to an Entity that already have it.");

        // Add component to the slotmap (copied or moved depending on the value category)
        key = storage.push_back(std::forward<CMP>(cmp));
        //Add key to the Entity
        e.template addComponent<T>(key);

        return storage[key];
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename CMP>
    void EntityManager<CMPS, TAGS, Capacity>::removeComponent(Entity& e) noexcept
    {
        debugAssertion(e.template hasComponent<CMP>(), "Trying to remove a component from an Entity that doesn`t have it.");

        Slotmap_t<CMP>& storage = components_.template getStorage<CMP>();
        cmp_key<CMP> key = e.template getComponentKey<CMP>();

        // Remove CMP from the slotmap
        storage.erase(key);

        e.template removeComponent<CMP>();
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename... CMPs>
    void EntityManager<CMPS, TAGS, Capacity>::removeComponents(Entity& e) noexcept
    {
        (removeComponent<CMPs>(e), ...);
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename TAG>
    void EntityManager<CMPS, TAGS, Capacity>::addTag(Entity& e) noexcept
    {
        debugAssertion(!e.template hasTag<TAG>(), "Trying to add a tag from an Entity that already have it.");
        e.template addTag<TAG>();
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename... TAGs>
    void EntityManager<CMPS, TAGS, Capacity>::addTags(Entity& e) noexcept
    {
        (addTag<TAGs>(e), ...);
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename TAG>
    void EntityManager<CMPS, TAGS, Capacity>::removeTag(Entity& e) noexcept
    {
        debugAssertion(e.template hasTag<TAG>(), "Trying to remove a tag from an Entity that doesn`t have it.");

        e.template removeTag<TAG>();
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename... TAGs>
    void EntityManager<CMPS, TAGS, Capacity>::removeTags(Entity& e) noexcept
    {
        (removeTag<TAGs>(e), ...);
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename CMP>
    CMP& EntityManager<CMPS, TAGS, Capacity>::getComponent(const Entity& e) noexcept
    {
        debugAssertion(e.template hasComponent<CMP>(), "Trying to get a component from an Entity that doesn`t have it.");

        cmp_key<CMP> key = e.template getComponentKey<CMP>();
        Slotmap_t<CMP>& storage = components_.template getStorage<CMP>();

        return storage[key];
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename CMP>
    const CMP& EntityManager<CMPS, TAGS, Capacity>::getComponent(const Entity& e) const noexcept
    {
        debugAssertion(e.template hasComponent<CMP>(), "Trying to get a component from an Entity that doesn`t have it.");

        cmp_key<CMP> key = e.template getComponentKey<CMP>();
        const Slotmap_t<CMP>& storage = components_.template getStorage<CMP>();

        return storage[key];
    }

} // namespace HeatStroke