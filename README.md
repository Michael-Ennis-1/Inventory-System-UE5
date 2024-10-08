# Inventory-System-UE5
This is a conversion project of a specific gameplay mechanic created in Blueprints in Unreal to C++, specifically a modular inventory system. 

![World_View](https://github.com/user-attachments/assets/a152e5af-8e79-4106-a343-0b1e9783c226)

## Technical Explanation
The inventory system allows for the user to add to their inventory by pressing "E" on an item, automatically assigning the item to an existing stack within the inventory. Otherwise, if there is no existing stack available, a new stack will be created.

Any items already within the inventory can be dropped at any time by right clicking on them and pressing either "Drop 1" or "Drop All". Stacks are capable of being transferred by holding left mouse button and moving the stack icon to another space in the inventory grid, even if there is already another item filling that slot. If two stacks of the same item type are attempting to be swapped, the second of the two slots will fill up to the maximum stack size with that item, siphoning from the first slot.

*Image of open Inventory Screen*
![Inventory_Screen](https://github.com/user-attachments/assets/3f682652-927a-4d00-9a56-d652c8975da6)

There is also a chest system, allowing interaction between a primary and secondary inventory. This has all the same features as the original inventory, allowing for transferring of slots between two seperate inventories. Also, items inside the chest are saved upon closing of the UI, allowing for the player to store items in a different location to their usual inventory.

*Image of Chest Inventory Screen, besides Player Inventory Screen*
![Chest](https://github.com/user-attachments/assets/6f86d801-708f-4321-889d-0b4082570d82)

This inventory system makes use of an actor component as well as several blueprint-callable functions to enable quick swapping between different inventory systems. This component-based modular approach allows for any other object in the game world to have their own inventory, including potentially other players in a multiplayer game. 

## Links
Link to video demonstration of project: https://youtu.be/ybbXlI4o7wQ
