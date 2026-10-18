#pragma once

#include <vector>
#include <iostream>
#include <type_traits>
#include <utility>
#include <componentstorage.hpp>
#include <log.hpp>
#include <format>
#include <optional>

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

                using Key_Storage_t = std::array<Key, cmpinfo::size()>;
                
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
                    return keys[cmpinfo::template id<CMP>()];
                }

                inline cmpinfo::mask_type getCMPmask() const noexcept
                {
                    return cmp_mask;
                }

                inline taginfo::mask_type getTAGmask() const noexcept
                {
                    return tag_mask;
                }
                
                private:
                    template<typename CMP>
                    void addComponent(cmp_key<CMP> key)
                    {
                        cmp_mask |= cmpinfo::template mask<CMP>();

                        keys[cmpinfo::template id<CMP>()] = key;
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

            struct SystemCache
            {
                cmpinfo::mask_type cmp_mask;
                taginfo::mask_type tag_mask;

                std::vector<std::size_t> ids{};
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

            Entity* getEntityById(const std::size_t id) noexcept;

            template<typename SYS>
            void registerSystem() noexcept;

            template<typename SYS>
            void foreach(auto&&) noexcept;


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

                using SystemCacheContainer_t = std::vector<SystemCache>;

                SystemCacheContainer_t caches_{};
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

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    typename EntityManager<CMPS, TAGS, Capacity>::Entity* EntityManager<CMPS, TAGS, Capacity>::getEntityById(const std::size_t id) noexcept
    {
        for(Entity& e : entities_)
        {
            if(e.getId() == id)
            {
                return &e;
            }
        }
        return nullptr;
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename SYS>
    void EntityManager<CMPS, TAGS, Capacity>::registerSystem() noexcept
    {
        using SystemCMPs = typename SYS::SCMPs;
        using SystemTAGs = typename SYS::STAGs;

        compileTimeAssertion(CMPS::template includes(SystemCMPs{}) , "System signature contains a nonpresent CMP type");
        compileTimeAssertion(TAGS::template includes(SystemTAGs{}), "System signature contains a nonpresent TAG type");
        // Compute system masks
        constexpr typename cmpinfo::mask_type cmp_mask0 = cmpinfo::template mask(SystemCMPs{});
        constexpr typename taginfo::mask_type tag_mask0 = taginfo::template mask(SystemTAGs{});

        Log::print(std::format("CMP mask {:#b}", cmp_mask0));
        Log::print(std::format("TAG mask {:#b}", tag_mask0));


        // Checks is there is a chache that already mach the signature
        bool finded = false;
        std::size_t cache_index = 0;

        for(std::size_t i{}; !finded && i < caches_.size(); ++i)
        {
            const SystemCache& cache = caches_[i];

            const bool mached_cmp_mask = (cache.cmp_mask  == cmp_mask0);
            const bool mached_tag_mask = (cache.tag_mask  == tag_mask0);
            if(mached_cmp_mask && mached_tag_mask )
            {
                cache_index = i;
                finded = true;
            }
        }

        if(!finded)
        {
            // Add the cache to cache_
            SystemCache& cache = caches_.emplace_back();
            cache_index = caches_.size() - 1;

            cache.cmp_mask = cmp_mask0;
            cache.tag_mask = tag_mask0;

            for(const Entity& e : entities_)
            {
                const bool contains_cmps = ((e.getCMPmask() & cache.cmp_mask) == cache.cmp_mask);
                const bool contains_tags = ((e.getTAGmask() & cache.tag_mask) == cache.tag_mask);
                if(contains_cmps && contains_tags)
                {
                    cache.ids.push_back(e.getId());
                    Log::print(("Entity with id: " + std::to_string(e.getId()) + " added to cache."));
                }
            }
        }
        // Update the system index
        SYS::cache_index = cache_index;
    }

    template<typename CMPS, typename TAGS, std::size_t Capacity>
    template<typename SYS>
    void EntityManager<CMPS, TAGS, Capacity>::foreach(auto&& func) noexcept
    {
        debugAssertion(SYS::cache_index < caches_.size(), "Index of system is out of bounds");

        const SystemCache& precached = caches_[SYS::cache_index];

        
    }

} // namespace HeatStroke