#pragma once

#include <vector>

namespace HeatStroke
{
    /*
     * EntityManager will provide a way to create and manage entities and their components.
     */
    template<typename... CMPS, std::size_t MAX_CMPS = 1000>
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

            /*
             * Adds a component to the specified entity.
             */
            template<typename CMP>
            CMP& addComponent(Entity& entity, CMP component);

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
                static std::size_t ID_COUNTER;

                Entity();

                inline std::size_t getId() const;

                // Entity data members
                std::size_t id;

                
            };
        private:
            std::vector<Entity> entities;
    };
} // namespace HeatStroke