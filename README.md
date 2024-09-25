# Inventory-System-UE5
This is a conversion project of a specific gameplay mechanic created in Blueprints in Unreal to C++, specifically a modular inventory system. 

![World_View](https://github.com/user-attachments/assets/a152e5af-8e79-4106-a343-0b1e9783c226)

## Technical Explanation
The inventory system allows for the user to add to their inventory by pressing "E" on an item, automatically assigning the item to an existing stack within the inventory. Otherwise, if there is no existing stack available, a new stack will be created.

Any items already within the inventory can be dropped at any time by right clicking on them and pressing either "Drop 1" or "Drop All". Stacks are capable of being transferred by holding left mouse button and moving the stack icon to another space in the inventory grid, even if there is already another item filling that slot. If two stacks of the same item type are attempting to be swapped, the second of the two slots will fill up to the maximum stack size with that item, siphoning from the first slot.

## Links
Link to video demonstration of project: https://youtu.be/ybbXlI4o7wQ
