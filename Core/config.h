#define WorldWidth 3
#define WorldHeight 3
#define MapWidth 40
#define MapHeight 15

// How big the gaps are between letters in rendered text in proportion to the letters themselves
#define LetterSpacing .1
// Scale of the dialogue box in proportion to the total available window space
#define DialogueBoxScale .2
// How many lines of text the dialogue box contains
#define DialogueBoxLines 3
// Amount of space between each line of text in proportion to the total size of the dialogue box
#define DialogueBoxLineSpacing .05

// Maximum entities that can exist at a time (not counting player)
#define MaxEntities 25
// Maximum number of entities that can be stored within a map
#define MaxEntitiesStoreable 10
// Amount of sheep the player must care for
#define MaxSheep 5

#define ShepherdHealth 3
#define FireballSpeed 3

// How many times a second the game ticks; essentially sets "speed" of gameplay
#define TickRate 10