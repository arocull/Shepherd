class Tile {
    public:
        Tile();
        Tile(int xPos, int yPos, int TileID);

    private:
        int x;
        int y;
        int id = 0;
        bool solid = false;
        bool liquid = false;

    public:
        int GetTileID();
        bool IsSolid();
        bool IsLiquid();

        void SetTileID(int newID);
        void SetPosition(int newX, int newY);
};