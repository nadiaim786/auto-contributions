// Learning Objective: Design a type-safe, template-based event bus system in C++.
// This tutorial will teach you how to create a central communication hub
// that allows different components to send and receive messages (events)
// without needing to know about each other directly. You will learn about:
// 1. Template metaprogramming for type identification.
// 2. Using std::function and lambdas for flexible callbacks.
// 3. Storing type-erased callbacks in a map.
// 4. Ensuring type safety during event subscription and emission.
// This system facilitates decoupled architecture, making your code more modular and maintainable.

#include <iostream>  // For console output (e.g., std::cout)
#include <functional> // For std::function, which stores callable entities (like lambdas or function pointers)
#include <map>        // For std::map, to store event listeners keyed by event type
#include <vector>     // For std::vector, to store multiple listeners for a single event type
#include <memory>     // Although not strictly used in this simplified version, useful for modern C++ resources

// EventBus class: Manages the subscription and emission of various event types.
// It acts as a central dispatcher for events in your application.
class EventBus {
private:
    // A map where:
    // - The key (void*) uniquely identifies an event type (e.g., PlayerMovedEvent, EnemySpawnedEvent).
    //   We use a 'void*' obtained from a static function to get a unique address per type.
    // - The value (std::vector<std::function<void(const void*)>>) is a list of
    //   type-erased listener functions. Each function takes a 'const void*'
    //   which will actually point to the event object being emitted.
    std::map<void*, std::vector<std::function<void(const void*)>>> listeners;

    // getTypeKey<TEvent>()
    // This template function provides a unique address for each distinct event type TEvent.
    // It leverages the fact that a static variable's address is unique within the program
    // and distinct for each specialization of the template.
    // This address serves as the key in our 'listeners' map to identify event types.
    template<typename TEvent>
    static void* getTypeKey() {
        static char key; // A dummy static variable, its address is unique per TEvent type.
        return &key;     // Return the address, which acts as our unique type identifier.
    }

public:
    // subscribe<TEvent>(handler)
    // Allows a component to register a callback function (handler) for a specific event type TEvent.
    // The 'handler' function will be called whenever an event of type TEvent is emitted.
    template<typename TEvent>
    void subscribe(std::function<void(const TEvent&)> handler) {
        // Get the unique key for this event type.
        void* key = getTypeKey<TEvent>();

        // Wrap the user's 'handler' into a std::function that accepts 'const void*'.
        // This wrapper performs the necessary type-safe casting internally.
        // When an event is emitted, its address (as const void*) is passed to this wrapper.
        // The wrapper then casts it back to the expected 'const TEvent*' and calls the original 'handler'.
        listeners[key].push_back([handler](const void* eventPtr) {
            // Safely cast the 'const void*' back to 'const TEvent*'.
            // We know this cast is safe because 'eventPtr' was originally a 'const TEvent*'.
            handler(*static_cast<const TEvent*>(eventPtr));
        });

        std::cout << "Subscribed to event type: " << typeid(TEvent).name() << std::endl;
    }

    // emit<TEvent>(event)
    // Publishes an event of type TEvent to all registered listeners.
    // The EventBus iterates through all handlers subscribed to TEvent and invokes them,
    // passing a constant reference to the 'event' object.
    template<typename TEvent>
    void emit(const TEvent& event) {
        // Get the unique key for this event type.
        void* key = getTypeKey<TEvent>();

        // Check if there are any listeners registered for this specific event type.
        auto it = listeners.find(key);
        if (it != listeners.end()) {
            std::cout << "Emitting event: " << typeid(TEvent).name() << std::endl;
            // Iterate over all handlers registered for TEvent.
            // Pass the address of the 'event' object as 'const void*' to each handler.
            for (auto& handler : it->second) {
                handler(static_cast<const void*>(&event));
            }
        } else {
            std::cout << "No listeners for event type: " << typeid(TEvent).name() << std::endl;
        }
    }

    // unsubscribe<TEvent>(...) - Optional, not implemented to keep tutorial focused and brief.
    // Implementing unsubscribe would require storing a unique identifier for each subscription
    // (e.g., a shared_ptr, or a token returned by subscribe) to allow removal of specific handlers.
};

// --- Example Event Structures ---
// Define simple struct to represent different types of events.
// Events are plain data structures that carry information.

struct PlayerMovedEvent {
    int x, y;
    std::string playerName;
};

struct EnemySpawnedEvent {
    int enemyID;
    float health;
    std::string type;
};

struct GameStateChangedEvent {
    std::string newState;
};

// --- Main function with example usage ---
int main() {
    std::cout << "--- Event Bus Tutorial Example ---" << std::endl << std::endl;

    // Create an instance of our EventBus.
    EventBus gameEventBus;

    // --- Subscription Examples ---
    std::cout << "--- Subscribing Listeners ---" << std::endl;

    // Subscribe a lambda function to PlayerMovedEvent.
    // The lambda captures 'this' implicitly if using class members, but here it's stateless.
    gameEventBus.subscribe<PlayerMovedEvent>([](const PlayerMovedEvent& event) {
        std::cout << "[Listener] Player '" << event.playerName << "' moved to ("
                  << event.x << ", " << event.y << ")." << std::endl;
    });

    // Subscribe another lambda to EnemySpawnedEvent.
    gameEventBus.subscribe<EnemySpawnedEvent>([](const EnemySpawnedEvent& event) {
        std::cout << "[Listener] Enemy '" << event.type << "' (ID: " << event.enemyID
                  << ") spawned with " << event.health << " health." << std::endl;
    });

    // Subscribe multiple listeners to the same event type (PlayerMovedEvent).
    // This demonstrates that multiple components can react to the same event.
    gameEventBus.subscribe<PlayerMovedEvent>([](const PlayerMovedEvent& event) {
        std::cout << "[Another Listener] Notifying UI that " << event.playerName << " changed position." << std::endl;
    });

    // Subscribe to a GameStateChangedEvent.
    gameEventBus.subscribe<GameStateChangedEvent>([](const GameStateChangedEvent& event) {
        std::cout << "[Listener] Game state changed to: " << event.newState << std::endl;
    });

    std::cout << std::endl;

    // --- Emission Examples ---
    std::cout << "--- Emitting Events ---" << std::endl;

    // Create and emit a PlayerMovedEvent.
    // Only listeners subscribed to PlayerMovedEvent will receive this.
    PlayerMovedEvent playerMove1 = {10, 20, "Hero"};
    gameEventBus.emit(playerMove1); // The type TEvent (PlayerMovedEvent) is deduced here.

    std::cout << "---" << std::endl;

    // Create and emit an EnemySpawnedEvent.
    EnemySpawnedEvent goblinSpawn = {101, 50.0f, "Goblin"};
    gameEventBus.emit(goblinSpawn);

    std::cout << "---" << std::endl;

    // Emit another PlayerMovedEvent. Both PlayerMovedEvent listeners will be called.
    PlayerMovedEvent playerMove2 = {15, 25, "Hero"};
    gameEventBus.emit(playerMove2);

    std::cout << "---" << std::endl;

    // Emit a GameStateChangedEvent.
    GameStateChangedEvent stateChange = {"LevelCompleted"};
    gameEventBus.emit(stateChange);

    std::cout << "---" << std::endl;

    // Emit an event type that has no registered listeners.
    // The EventBus will correctly identify that there are no handlers.
    struct NonExistentEvent {}; // A new event type not subscribed to.
    NonExistentEvent noListenersEvent;
    gameEventBus.emit(noListenersEvent);

    std::cout << std::endl << "--- Example Finished ---" << std::endl;

    return 0;
}