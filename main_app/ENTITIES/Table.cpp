#include "Table.h"

// Table constructor
Table::Table() : texture("asset/table.png"),
                 sprite(texture)

{
    gameAreaSize = {1196, 673};
    tableOffset = {0.f, 0.f};
}

void Table::setup(sf::Vector2u windowSize)
{
    sf::Vector2u textureSize = texture.getSize();

    // center offset
    float offsetX = (windowSize.x - gameAreaSize.x) / 2.0f;
    float offsetY = (windowSize.y - gameAreaSize.y) / 2.0f;
    this->tableOffset = {offsetX, offsetY};

    // scale
    float mathScaleX = static_cast<float>(gameAreaSize.x) / textureSize.x;
    float scaleY     = static_cast<float>(gameAreaSize.y) / textureSize.y;

    // sprite setup
    sprite.setScale({-mathScaleX, scaleY});
    sprite.setPosition({tableOffset.x + gameAreaSize.x, tableOffset.y}); 

    // cushions
    float margin = CUSHION_WIDTH * mathScaleX; 
    float gap = POCKET_SIZE * mathScaleX;
    float tableW = static_cast<float>(gameAreaSize.x);
    float tableH = static_cast<float>(gameAreaSize.y);

    cushions.clear();

    float cornerInset = margin + (gap * 0.4f); 
    float topLeftStart = margin + cornerInset; 
    float middleOfTable = tableW / 2.0f;
    float middlePocketEdge = middleOfTable - (gap / 2.0f);
    float topLeftWidth = middlePocketEdge - topLeftStart;

    // Top left
    cushions.push_back({{tableOffset.x + topLeftStart, tableOffset.y + margin}, 
                        {topLeftWidth, margin}});

    // Top right
    float topRightStart = middleOfTable + (gap / 2.6f);
    float tableRightEdge = tableW - margin;
    float topRightEnd = tableRightEdge - cornerInset;
    float topRightWidth = topRightEnd - topRightStart;

    cushions.push_back({{tableOffset.x + topRightStart, tableOffset.y + margin}, 
                        {topRightWidth, margin}});

    // Bottom left
    cushions.push_back({{tableOffset.x + topLeftStart, tableOffset.y + tableH - (margin * 2.0f)}, 
                        {topLeftWidth, margin}});

    // Bottom right
    cushions.push_back({{tableOffset.x + topRightStart, tableOffset.y + tableH - (margin * 2.0f)}, 
                        {topRightWidth, margin}});

    // Sides
    float sideSegmentHeight = tableH - (margin * 1.9f) - (gap * 1.9f); 
    float sideTopStart = margin + (gap * 0.9f); 

    cushions.push_back({
        {tableOffset.x + margin, tableOffset.y + sideTopStart}, 
        {margin, sideSegmentHeight} 
    });

    cushions.push_back({
        {tableOffset.x + tableW - (margin * 2.0f), tableOffset.y + sideTopStart}, 
        {margin, sideSegmentHeight}
    });

    // pockets
    pockets.clear();
    float cornerOffset = margin + (gap * 0.5f); 
    float midX = tableW / 2.0f;
    float midY_Top = margin + (gap * 0.1f); 
    float midY_Bot = tableH - margin - (gap * 0.1f);

    pockets.push_back({tableOffset.x + cornerOffset, tableOffset.y + cornerOffset});             
    pockets.push_back({tableOffset.x + midX,         tableOffset.y + midY_Top});                         
    pockets.push_back({tableOffset.x + tableW - cornerOffset, tableOffset.y + cornerOffset});    
    pockets.push_back({tableOffset.x + cornerOffset, tableOffset.y + tableH - cornerOffset});    
    pockets.push_back({tableOffset.x + midX,         tableOffset.y + midY_Bot});                         
    pockets.push_back({tableOffset.x + tableW - cornerOffset, tableOffset.y + tableH - cornerOffset}); 
}
sf::Sprite &Table::getSprite() { return sprite; }
const std::vector<sf::FloatRect> &Table::getCushions() { return cushions; }
const std::vector<sf::Vector2f>& Table::getPockets() { return pockets; }