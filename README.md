# About

**TrickyInteractionSystem** provides a simple but flexible interaction system for Unreal Engine 5 projects.

## Features

* **Interaction Queue System**: Automatically maintains and sorts interactive objects based on priority
* **Line of Sight Detection**: Optional requirement that objects must be in view to interact with them
* **Comprehensive Interface**: Simple Blueprint-friendly interface for creating interactive objects
* **Versatile Interaction Methods**: Support for starting, finishing, interrupting, and forcing interactions

## Installation

1. Copy the plugin folder into your project's `Plugins` directory.
2. Open your Unreal Engine project.
3. Enable the **TrickyInteractionSystem** in the plugin menu.
4. Restart the editor.

## Basic Setup

1. Add to your player actor `UInteractionQueueComponent`.
2. Make new interactive actor (or update an existing one) by:
   * Creating a variable named `InteractionData` of `FInteractionData` type 
   * Implementing `ITrickyInteractionInterface` to the actor
3. Add this actor to interaction queue using static functions from `UTrickyInteractionLbirary` or directly from the component
4. Setup Interaction controls to call interaction functions

## Functions

### InteractionQueueComponent
The `UInteractionQueueComponent` is an Actor Component responsible for managing a queue of interactive actors for its owner.

**Key Functions:**
*   `AddToInteractionQueue(AActor* InteractiveActor)`: Adds an interactive actor to the queue.
*   `RemoveFromInteractionQueue(AActor* InteractiveActor)`: Removes an interactive actor from the queue.
*   `IsInInteractionQueue(AActor* Actor)`: Checks if a specific actor is currently in the queue.
*   `StartInteraction()`: Attempts to start an interaction with the highest priority actor in the queue.
*   `FinishInteraction()`: Attempts to finish the current interaction.
*   `InterruptInteraction(AActor* Interruptor)`: Attempts to interrupt the current interaction.
*   `ForceInteraction()`: Forces an interaction with the highest priority actor, typically for immediate interactions.
*   `RegisterCamera(UCameraComponent* Camera)`: Registers a camera component to be used for Line of Sight checks.
*   `SetUseLineOfSight(bool Value)`: Enables or disables the Line of Sight requirement for interactions.

**Key Properties:**
*   `InteractionQueue (TArray<AActor*>)`: The current list of interactive actors, sorted by priority. (Getter: `GetInteractionQueue`)
*   `bUseLineOfSight (bool)`: If true, Line of Sight checks are performed. (Getter: `GetUseLineOfSight`, Setter: `SetUseLineOfSight`)
*   `TraceChannel (ETraceTypeQuery)`: The trace channel used for Line of Sight checks.
*   `LineOfSightDistance (float)`: The maximum distance for Line of Sight checks.
*   `LineOfSightRadius (float)`: The radius of the sphere trace used for Line of Sight checks.

**Delegates:**
*   `OnActorAddedToInteractionQueue`: Called when an actor is added to the queue.
*   `OnActorRemovedFromInteractionQueue`: Called when an actor is removed from the queue.
*   `OnInteractionStarted`: Called when an interaction attempt is made.
*   `OnInteractionFinished`: Called when a finish interaction attempt is made.
*   `OnInteractionInterrupted`: Called when an interrupt interaction attempt is made.
*   `OnInteractionForced`: Called when a force interaction attempt is made.

### Interaction Interface
The `ITrickyInteractionInterface` must be implemented by any actor that wishes to be interactive.

**Functions to Implement:**
*   `StartInteraction(AActor* Interactor)`: Called when an interactor starts an interaction with this object.
*   `InterruptInteraction(AActor* Interruptor, AActor* Interactor)`: Called when an interactor attempts to interrupt an ongoing interaction.
*   `FinishInteraction(AActor* Interactor)`: Called when an interactor finishes an interaction with this object.
*   `ForceInteraction(AActor* Interactor)`: Called when an interactor forces an interaction with this object.

### InteractionData Struct
The `FInteractionData` struct holds information about how an actor can be interacted with. It should be added as a UPROPERTY to interactive actors with the name "InteractionData".

**Members:**
*   `InteractionMessage (FText)`: Text displayed to the player (e.g., "Press E to Interact"). Defaults to "Interact".
*   `bRequiresLineOfSight (bool)`: If true, this object can only be interacted with if it's in the player's line of sight. Defaults to `false`.
*   `InteractionWeight (int32)`: Determines the priority in the interaction queue. Higher values mean higher priority. Ignored if `bRequiresLineOfSight` is true for the `UInteractionQueueComponent`. Defaults to `0`.

### TrickyInteractionLibrary
`UTrickyInteractionLibrary` provides static Blueprint utility functions for the interaction system.

**Key Functions:**
*   `IsActorInteractive(AActor* Actor)`: Checks if an actor implements `ITrickyInteractionInterface` and has valid `InteractionData`.
*   `GetActorInteractionData(AActor* Actor, FInteractionData& InteractionData)`: Retrieves the `FInteractionData` from an interactive actor.
*   `AddToInteractionQueue(AActor* Interactor, AActor* InteractiveActor)`: Adds an interactive actor to the specified interactor's queue.
*   `RemoveFromInteractionQueue(AActor* Interactor, AActor* InteractiveActor)`: Removes an interactive actor from the specified interactor's queue.
*   `GetInteractionQueueComponent(const AActor* Actor)`: Gets the `UInteractionQueueComponent` from a given actor, if it exists.
*   `IsInInteractionQueue(const AActor* Interactor, AActor* Actor)`: Checks if an actor is in the specified interactor's queue.
