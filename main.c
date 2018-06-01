//
//  main.cpp
//  BattleShip
//
//  Created by Filip on 22/05/16.
//  Copyright © 2016 Filip. All rights reserved.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum { false, true } bool;

/* ############### KONSTANTE ##########################  */

// dimenzije ploce
#define BOARD_WIDTH  10
#define BOARD_HEIGHT  10

#define PLAYER_HUMAN 1
#define PLAYER_COMPUTER 2

#define isWATER '~'
#define isHIT '*'
#define isMISS 'm'

#define SHIP_TYPES 5

/* ################## STRUKTURE ########################## */
struct POINT //koordinata
{
    //Lokacija na polju definirana s x(horizontalna) i y(vertikalna) koordinatama
    
    int X;
    int Y;
};

struct SHIP
{
    char name[30];
    char symbol; // {D,S,R,B,C}
    int length;
    struct POINT onGrid[5]; //naPolju
    bool hitFlag[5];
} ship[SHIP_TYPES];

struct PLAYER
{
    char grid[BOARD_WIDTH][BOARD_HEIGHT];
} player[3]; //ignoriraj player 0, koristi player 1 ili 2

typedef enum {HORIZONTAL, VERTICAL} DIRECTION; //MG : Zamjeniti u neku drugu listu

struct PLACESHIP
{
    DIRECTION direction;
    struct SHIP shipType;
} placeShip;

bool gameRunning = false; //KOMENTIRAJ


/* ##################  FUNKCIJE ########################## */


void LoadShips()
{
    
    //KOMENTIRAJ
    
    strcpy(ship[0].name, "razarac");
    strcpy(ship[1].name, "podmornica");
    strcpy(ship[2].name, "krstarica");
    strcpy(ship[3].name, "vojni brod");
    strcpy(ship[4].name, "nosac zrakoplova");
    
    ship[0].length = 2;
    ship[1].length = 3;
    ship[2].length = 3;
    ship[3].length = 4;
    ship[4].length = 5;
    
    ship[0].symbol = 'D';
    ship[1].symbol = 'S';
    ship[2].symbol = 'R';
    ship[3].symbol = 'B';
    ship[4].symbol = 'C';
    
    
}

void ResetBoard()
{
    //Loop through each player
    for (int iPlayer = 1; iPlayer < 3; iPlayer++)
    {
        //For each grid point, set contents to 'water'
        for (int w = 0; w < BOARD_WIDTH; w++)
        {
            for (int h = 0; h < BOARD_HEIGHT; h++)
            {
                player[iPlayer].grid[w][h] = isWATER;
            }
        }
    }
}

bool isShip( char c )
{
    bool result = true;
    if ( ( c == isWATER ) || ( c == isMISS ) || ( c == isHIT ) )
    {
        result = false;
    }
    return result;
}

void DrawBoard(int currentPlayer)
{
    printf("PLAYER  %d  GAME BOARD \n", currentPlayer);
    printf("  ");
    
    for ( int iWidth = 0 ; iWidth < BOARD_WIDTH; iWidth++ )
    {
        printf("%d  ", iWidth);
    }
    printf("\n");
    for ( int h = 0 ; h < BOARD_HEIGHT; h++ )
    {
        for ( int w = 0; w < BOARD_WIDTH; w++ )
        {
            if ( w == 0 ) printf("%d ", h); //prikazi broj reda
            if ( gameRunning == false ) printf("%c  ",  player[currentPlayer].grid[w][h]);
            if ( ( gameRunning == true ) && ( isShip( player[currentPlayer].grid[w][h] ) == false ) )
            {
                printf("%c  ", player[currentPlayer].grid[w][h]);
            }
            else if ( gameRunning == true && ( isShip( player[currentPlayer].grid[w][h] ) == true ) )
            {
                if ( currentPlayer ==  PLAYER_COMPUTER )
                {
                    // ako je player kompjuter,skrivamo brod sa simbolom vode
                    printf("%c  ", isWATER);
                }
                else
                {
                    printf("%c  ", player[currentPlayer].grid[w][h]);
                }
            }
            if (w == BOARD_WIDTH-1) printf("\n");
        }
    }
}

struct PLACESHIP UserInputShipPlacement()
{
    int d, x, y;
    struct PLACESHIP tmp;
    tmp.shipType.onGrid[0].X = -1;
    do
    {
        printf("direction [0 = horizontal, 1=vertical]: ");
        scanf("%d", &d);
    } while ( d != 1 && d!= 0 );
    printf("\n");
    do
    {
        printf("column # : ");
        scanf("%d", &x);
    } while ( x < 0 && x > BOARD_WIDTH );
    printf("\n");
    do
    {
        printf("row # : ");
        scanf("%d", &y);
    } while ( y < 0 && y > BOARD_HEIGHT );
    printf("valid data entered\n");
    tmp.direction = (DIRECTION)d;
    tmp.shipType.onGrid[0].X = x;
    tmp.shipType.onGrid[0].Y = y;
    return tmp;
}

DIRECTION getRandomDirection() //generira random broj 0 ili 1,za određivanje položaja broda kompjutera-horizontalno/vertikalno
{
    srand(time(0));
    int dir = rand() % 2;
    if ( dir == 0 )
    {
        return HORIZONTAL;
    }
    else
    {
        return VERTICAL;
    }
}

int getRandBetween( int min, int max)
{
    return min + (rand() % (int)(max - min + 1));
}

int getEvenRandBetween( int min, int max ) // korišteno kasnije u "pametnom" algoritmu
{
    srand(time(0));
    return 2 * ( min + ( rand() %  ( max / 2 - min + 1) )   );
}

struct PLACESHIP ComputerInputShipPlacement()
{
    struct PLACESHIP tmp;
    tmp.shipType.onGrid[0].X = -1;
    tmp.direction = getRandomDirection();
    tmp.shipType.onGrid[0].X = getRandBetween( 0 , BOARD_WIDTH - 1 );
    tmp.shipType.onGrid[0].Y = getRandBetween( 0 , BOARD_HEIGHT - 1 );
    return tmp;
}

bool checkOverlap( int x, int y, DIRECTION d, int length, int iPlayer )
{
    bool result = true;
    
    //for ( int i = 1; ( i < length ) && result ; i++ ) // start at i = 1 as we don't check for the first position
    for ( int i = 0; ( i < length ) && result ; i++ )
    {
        if ( player[ iPlayer ].grid[ x ][ y ] != isWATER ) result = false;
        
        if ( d == HORIZONTAL )
        {
            
            x++;
        }
        if ( d == VERTICAL )
        {
            y++;
        }
        
    }
    return result;
}

bool checkIsInsideBorders( int x, int y, DIRECTION d, int length, int iPlayer )
{
    if ( ( ( d == VERTICAL ) && ( y + length ) > BOARD_HEIGHT ) ||
        ( ( d == HORIZONTAL ) && ( x + length ) >  BOARD_WIDTH) )
    {
        return false;
    } else return true;
}

void instructHumanPlayer( char shipName[], int shipLength )
{
    printf("\n");
    printf("INSTRUCTIONS (PLAYER 1)\n\n");
    printf("You are about to place your ships. Format should be:\n");
    printf("Facing (0:Horizontal,1:Vertical), X(top-row) coords, Y(left-side) coords\n");
    printf("Ship to place: %s which has a length of %d\n", shipName, shipLength);
    printf("Where do you want it placed? ");
}

void generateBoards()
{
    bool noOverlap = true;
    bool isInsideBorders = true;
    for ( int iPlayer = 1 ; iPlayer < 3; iPlayer++ )
    {
        for ( int iShip = 0; iShip < SHIP_TYPES; iShip++)
        {
            // Display board
            system("clear");
            if( iPlayer == 1)
            {
                DrawBoard(iPlayer);
                instructHumanPlayer( ship[iShip].name, ship[iShip].length);
            }
            else
                printf("Computer is placing its ships, please wait …\n");
            struct PLACESHIP aShip;
            aShip.shipType.onGrid[0].X = -1;
            noOverlap = true;
            isInsideBorders = true;
            while ( aShip.shipType.onGrid[0].X == -1 )
            {
                if ( iPlayer == 1 )
                {
                    aShip = UserInputShipPlacement();
                }
                else
                {
                    aShip = ComputerInputShipPlacement();
                }
                noOverlap = checkOverlap( aShip.shipType.onGrid[0].X, aShip.shipType.onGrid[0].Y,
                                         aShip.direction, ship[iShip].length, iPlayer );
                isInsideBorders = checkIsInsideBorders( aShip.shipType.onGrid[0].X, aShip.shipType.onGrid[0].Y,
                                                       aShip.direction, ship[iShip].length, iPlayer );
                if ( !noOverlap )
                {
                    if ( iPlayer == 1 ) printf(" the ship is overlapping with another one\n");
                    aShip.shipType.onGrid[0].X = -1;
                }
                if ( !isInsideBorders )
                {
                    if ( iPlayer == 1 )
                        printf(" the ship goes out of the border limits\n");
                    aShip.shipType.onGrid[0].X = -1;
                }
            }
            
            aShip.shipType.length = ship[iShip].length;
            strcpy(aShip.shipType.name, ship[iShip].name);
            
            
            //Add the FIRST grid point to the current player's game board
            player[iPlayer].grid[aShip.shipType.onGrid[0].X][aShip.shipType.onGrid[0].Y] = ship[iShip].symbol;
            
            //Add the rest of the symbols to the current player's game board
            for ( int i = 1; i < aShip.shipType.length; i++ )
            {
                if ( aShip.direction == HORIZONTAL )
                {
                    aShip.shipType.onGrid[i].X = aShip.shipType.onGrid[i-1].X+1;
                    aShip.shipType.onGrid[i].Y = aShip.shipType.onGrid[i-1].Y;
                }
                if (aShip.direction == VERTICAL)
                {
                    aShip.shipType.onGrid[i].Y = aShip.shipType.onGrid[i-1].Y+1;
                    aShip.shipType.onGrid[i].X = aShip.shipType.onGrid[i-1].X;
                }
                
                player[iPlayer].grid[aShip.shipType.onGrid[i].X][aShip.shipType.onGrid[i].Y] = ship[iShip].symbol;
                
            }
        }
        system("clear"); // OS X terminal command
    }
}

bool checkIfPositionIsShipOccupied( int i, int j, int currentPlayer )
{
    
    
    if ( isShip(player[currentPlayer].grid[i][j]) )
    {
        
        return true;
    } else return false;
    
}

int shipsCounter[2][5];

void initShipsCounter( int currentPlayer )
{
    shipsCounter[currentPlayer-1][0]=2;
    shipsCounter[currentPlayer-1][1]=3;
    shipsCounter[currentPlayer-1][2]=3;
    shipsCounter[currentPlayer-1][3]=4;
    shipsCounter[currentPlayer-1][4]=5;
}

void updateShipsCounter( int tempShipCounter[5], int currentPlayer )
{
    for ( int iShipType = 0; iShipType < SHIP_TYPES; iShipType++)
    {
        shipsCounter[currentPlayer-1][iShipType] = tempShipCounter[iShipType];
    }
}

void countRemainingShipPieces( int currentPlayer )
{
    int tempShipCounter[5];
    for ( int k = 0; k < SHIP_TYPES; k++)
    {
        tempShipCounter[ k ] = 0;
    }
    for ( int i = 0; i < BOARD_WIDTH; i++ )
    {
        for ( int j = 0; j < BOARD_HEIGHT; j++ )
        {
            switch ( player[currentPlayer].grid[i][j] )
            {
                case 'D':
                    tempShipCounter[ 0 ]++;//D
                    break;
                case 'S':
                    tempShipCounter[ 1 ]++;//S
                    break;
                case 'R':
                    tempShipCounter[ 2 ]++;//R
                    break;
                case 'B':
                    tempShipCounter[ 3 ]++;//B
                    break;
                case 'C':
                    tempShipCounter[ 4 ]++;//C
                    break;
            }
        }
    }
    printf("over with counting\n");
    for ( int iShipType = 0; iShipType < SHIP_TYPES; iShipType++ )
    {
        
        if ( shipsCounter[currentPlayer - 1][iShipType] == 1
            && tempShipCounter[iShipType] == 0 )
        {
            switch (iShipType)
            {
                case 0:
                    printf("\nRazarac has been destroyed\n");
                    break;
                case 1:
                    printf("\nPodmornica has been destroyed\n");
                    break;
                case 2:
                    printf("\nKrstarica has been destroyed\n");
                    break;
                case 3:
                    printf("\nVojni brod has been destroyed\n");
                    break;
                case 4:
                    printf("\nNosac zrakoplova has been destroyed\n");
                    break;
            }
        }
    }
    updateShipsCounter(tempShipCounter, currentPlayer);
}

bool hasPlayerLost(int currentPlayer)
{
    
    int count = 0;
    
    for ( int i = 0; i < BOARD_WIDTH; i++ )
    {
        for ( int j = 0; j < BOARD_HEIGHT; j++ )
        {
            
            if ( checkIfPositionIsShipOccupied( i, j, currentPlayer ) )
            {
                count++;
            }
        }
    }
    if ( count > 0 )
    {
        //printf("PLAYER %d is still alive\n", currentPlayer);
        return false;
    }
    else
    {
        printf("PLAYER %d has lost the game!\n", currentPlayer);
        return true;
    }
}

bool isComputerLockingTarget = false;
int computerHitPosition[2];
bool smartComputerAlgorithmEnabled;
int lastHitPosition[2];
int targetCounter;
bool goNext;

void attackPosition( int i, int j )
{
    goNext = false;
    
    if ( player[PLAYER_HUMAN].grid[i][j] == isHIT || player[PLAYER_HUMAN].grid[i][j] == isMISS )
    {
        //printf(" this position has already been bombed, moving to the next possible position\n");
        goNext = true;
    }
    else
    {
        if ( checkIfPositionIsShipOccupied( i, j, PLAYER_HUMAN ) )
        {
            printf("Human's board got hit: %c, col=%d, row=%d", player[PLAYER_HUMAN].grid[i][j], i, j); // je li ovo isto kao ova linija ispod?
            //cout << "Human's board got hit : " << player[PLAYER_HUMAN].grid[i][j] << "(col=" << i << ",row=" << j << ")" << endl;
            player[PLAYER_HUMAN].grid[i][j] = isHIT;
            countRemainingShipPieces( PLAYER_HUMAN );
            system("clear");
            DrawBoard( PLAYER_COMPUTER );
            DrawBoard( PLAYER_HUMAN );
            lastHitPosition[0] = i;
            lastHitPosition[1] = j;
            //printf("next position updated, keeping target seek mode enabled.\n");
            //printf("next seek and destroy operation will start from col=%d,row=%d\n", i, j);
        }
        else //go to the next move, because the only remaining possibility is a hit
        {
            player[PLAYER_HUMAN].grid[i][j] = isMISS;
        }
    }
}

void doAttackRandomPosition()
{
    isComputerLockingTarget = false;
    int i,j, counterMax, counter;
    //printf("Searching randomly but smartly using the parity principle\n");
    counterMax = (int) BOARD_HEIGHT * BOARD_WIDTH / 2 ;
    counter = 0;
    do //the computer memorizes that is should not hit a useless position anymore
        // search for the first cell that is water or ship
    {
        if ( counter < counterMax ){
            // we hit only at even lines and columns (parity principle)
            j = getEvenRandBetween( 0 ,  BOARD_HEIGHT - 1 );
            i = getEvenRandBetween( 0 ,  BOARD_WIDTH - 1 );
            counter++;
        }
        else
        {
            j = getRandBetween( 0 ,  BOARD_HEIGHT - 1);
            i = getRandBetween( 0 ,  BOARD_WIDTH - 1 );
        }
    }
    while ( player[PLAYER_HUMAN].grid[i][j] == isHIT || player[PLAYER_HUMAN].grid[i][j] == isMISS );
    if ( checkIfPositionIsShipOccupied( i, j, PLAYER_HUMAN ) )
    {
        player[PLAYER_HUMAN].grid[i][j] = isHIT;
        countRemainingShipPieces( PLAYER_HUMAN);
        system("clear");
        DrawBoard( PLAYER_COMPUTER );
        DrawBoard( PLAYER_HUMAN );
        printf("Human's board got hit and computer starts a seek and destroy\n"); //moze proc ovo u zamjenu za liniju ispod?
        //cout << "Human's board got hit and computer starts a seek and destroy: " << player[PLAYER_HUMAN].grid[i][j] << endl;
        computerHitPosition[0] = i;
        computerHitPosition[1] = j;
        isComputerLockingTarget = true; // start of seek and destroy operation
        //compute the number of positions to hit : 3 if on border 4 else
        targetCounter = 0;
        lastHitPosition[0] = i;
        lastHitPosition[1] = j;
    }
    else
    {
        player[PLAYER_HUMAN].grid[i][j] = isMISS;
    }
    
}

void doAttack( int currentPlayer )
{
    int i, j;
    if ( ( currentPlayer == PLAYER_HUMAN ) && !hasPlayerLost(PLAYER_COMPUTER) && !hasPlayerLost(PLAYER_HUMAN) )
    {
        do
        {
            printf("Please enter the coordinates where you want to attack : ");
            scanf("%d %d", &i, &j);
            if(player[PLAYER_COMPUTER].grid[i][j] == isHIT ||
               player[PLAYER_COMPUTER].grid[i][j] == isMISS)
                printf("\nPosition already bombed, try again\n");
            // unosi koordinate dokle god su jednake * ili m
        }
        while( player[PLAYER_COMPUTER].grid[i][j] == isHIT ||
              player[PLAYER_COMPUTER].grid[i][j] == isMISS );
        printf("\n");
        if ( checkIfPositionIsShipOccupied( i, j, PLAYER_COMPUTER ) )
        {
            printf("Computer's board got hit : %c\n", player[PLAYER_COMPUTER].grid[i][j]);
            player[PLAYER_COMPUTER].grid[i][j] = isHIT;
            countRemainingShipPieces( PLAYER_COMPUTER );
            system("clear");
            DrawBoard( PLAYER_COMPUTER );
            DrawBoard( PLAYER_HUMAN );
        }
        else
        {
            player[PLAYER_COMPUTER].grid[i][j] = isMISS;
        }
        
    }
    else // PLAYER = COMPUTER
    {
        if ( smartComputerAlgorithmEnabled == false )
        {
            // Algoritam baziran na http://www.datagenetics.com/blog/december32011/
            do //the computer memorizes that is should not hit a useless position anymore
            {
                j = getRandBetween( 0 ,  BOARD_HEIGHT - 1);
                i = getRandBetween( 0 ,  BOARD_WIDTH - 1 );
            }
            while ( player[PLAYER_HUMAN].grid[i][j] == isHIT || player[PLAYER_HUMAN].grid[i][j] == isMISS );
            
            if ( checkIfPositionIsShipOccupied( i, j, PLAYER_HUMAN ) )
            {
                printf("Human's board got hit : %c\n",player[PLAYER_HUMAN].grid[i][j]);
                player[PLAYER_HUMAN].grid[i][j] = isHIT;
                countRemainingShipPieces( PLAYER_HUMAN );
                system("clear");
                DrawBoard( PLAYER_COMPUTER );
                DrawBoard( PLAYER_HUMAN );
            }
            else
            {
                player[PLAYER_HUMAN].grid[i][j] = isMISS;
            }
        }
        else
        {
            if ( isComputerLockingTarget == false )
            {
                doAttackRandomPosition();
            }
            else // PLAYER == COMPUTER and ship to attack is found ( isComputerLockingTarget == true )
            {
                // take the last direction
                // hit the target in that direction, next point such that
                // if we are at the border, we go in the opposite direction
                
                i = computerHitPosition[0];
                j = computerHitPosition[1];
                //printf("Computer still in locking target mode\n");
                //printf("current position : row = %d, col = %d", i, j);
                // POSITION 1: gornji lijevi kut
                if ( i == 0 && j == 0 )
                {
                    switch (targetCounter)
                    {
                        case 0 :
                            attackPosition( i + 1, j);
                            targetCounter++;
                            if ( !goNext ) break;
                        case 1:
                            attackPosition( i , j + 1);
                            targetCounter = 0;
                    }
                }
                // POSITION 2 : gornji desni kut
                if ( i == BOARD_WIDTH - 1 && j == 0 )
                {
                    switch (targetCounter)
                    {
                        case 0 :
                            attackPosition( i - 1, j);
                            targetCounter++;
                            if ( !goNext ) break;
                        case 1:
                            attackPosition( i , j + 1);
                            targetCounter = 0;
                    }
                }
                //POSITION 3 : donji lijevi kut
                if ( i == 0 && j == BOARD_HEIGHT - 1 )
                {
                    switch (targetCounter)
                    {
                        case 0 :
                            attackPosition( i , j - 1 );
                            targetCounter++;
                            if ( !goNext ) break;
                        case 1:
                            attackPosition( i + 1 , j );
                            targetCounter = 0;
                    }
                }
                //POSITION 4 : donji desni kut
                if ( i == BOARD_WIDTH - 1  && j == BOARD_HEIGHT - 1 )
                {
                    switch (targetCounter)
                    {
                        case 0 :
                            attackPosition( i , j - 1 );
                            targetCounter++;
                            if ( !goNext ) break;
                        case 1:
                            attackPosition( i - 1 , j );
                            targetCounter = 0;
                    }
                }
                //POSITION 5 : gornja granica
                if ( (j == 0) && (i >= 1) && i <= (BOARD_WIDTH - 1) )
                {
                    
                    switch (targetCounter) {
                        case 0:
                            attackPosition( i - 1, j);
                            targetCounter++;
                            if ( !goNext ) break;
                        case 1:
                            attackPosition( i + 1, j );
                            targetCounter++;
                            if ( !goNext ) break;
                        case 2:
                            attackPosition( i, j + 1);
                            targetCounter = 0;
                    }
                }
                //POSITION 6: donja granica
                if ( (j == BOARD_HEIGHT - 1) && (i >= 1) && i <= (BOARD_WIDTH - 1) )
                {
                    
                    switch (targetCounter) {
                        case 0:
                            attackPosition( i, j - 1);
                            targetCounter++;
                            if ( !goNext ) break;
                        case 1:
                            attackPosition( i + 1, j );
                            targetCounter++;
                            if ( !goNext ) break;
                        case 2:
                            attackPosition( i - 1, j );
                            targetCounter = 0;
                            
                    }
                }
                //POSITION 7: sredina
                if (( j < (BOARD_HEIGHT - 1) ) && j > 0 && i > 0 && i < ( BOARD_WIDTH - 1 ) )
                {
                    switch (targetCounter) {
                        case 0:
                            printf("attacking %d, %d\n", i, j-1);
                            printf("target counter = %d", targetCounter);
                            attackPosition( i, j - 1);
                            targetCounter++;
                            if ( !goNext ) break;
                        case 1:
                            printf("attacking %d, %d\n", i, j+1);
                            printf("target counter = %d", targetCounter);
                            attackPosition( i , j + 1 );
                            targetCounter++;
                            if ( !goNext ) break;
                        case 2:
                            printf("attacking %d, %d\n", i-1, j);
                            printf("target counter = %d", targetCounter);
                            attackPosition( i - 1, j );
                            targetCounter++;
                            if ( !goNext ) break;
                        case 3:
                            printf("attacking %d, %d\n", i+1, j);
                            printf("target counter = %d", targetCounter);
                            attackPosition( i + 1, j );
                            targetCounter = 0;
                            
                    }
                }
                
                if ( targetCounter == 0 )
                {
                    
                    if ( (computerHitPosition[0] != lastHitPosition[0]) || (computerHitPosition[1] != lastHitPosition[1]) )
                    {
                        // if the initial impact is different from the last impact made in locking mode, then we update the position
                        // and keep the target locking mode on.
                        /*printf("computerHitPosition[0] = %d, lastHitPosition[0] = %d\n",computerHitPosition[0], lastHitPosition[0] );
                        printf("computerHitPosition[1] = %d, lastHitPosition[1] = %d\n", computerHitPosition[1], lastHitPosition[1]);
                        printf("target counter = %d", targetCounter);
                        printf(" >>>> Updating the current position <<<< \n");*/
                        computerHitPosition[0] = lastHitPosition[0];
                        computerHitPosition[1] = lastHitPosition[1];
                    } else
                    {
                        isComputerLockingTarget = false;
                        printf("leaving target lock mode\n");
                    }
                }
                
                // Check if unlock is necessary : the condition is that there was no additional hit during the bombing
                // while the locking mode was enabled.
            }
        }
    }
}

int main(int argc, const char * argv[]) {
    
    ResetBoard();
    LoadShips();
    
    generateBoards();
    
    DrawBoard(PLAYER_HUMAN); //warning : naming convention and use constants for the player number
    //DrawBoard(PLAYER_COMPUTER);
    initShipsCounter(PLAYER_HUMAN);
    initShipsCounter(PLAYER_COMPUTER);
    char startTheGame;
    printf("Press any letter and Enter to start the game ...");
    scanf("%c", &startTheGame);
    
    smartComputerAlgorithmEnabled = false;
    
    int smartComputer;
    
    printf("Strong or Weak computer opponent? [0 = weak, 1 = Strong]");
    scanf("%d", &smartComputer);
    if (smartComputer == 1) smartComputerAlgorithmEnabled = true;
    
    gameRunning = true;
    printf("\nGAME START!!\n");
    system("clear");
    
    int counter;
    while ( !hasPlayerLost(PLAYER_COMPUTER) && !hasPlayerLost(PLAYER_HUMAN) )
    {
        DrawBoard( PLAYER_COMPUTER );
        DrawBoard( PLAYER_HUMAN );
        doAttack( PLAYER_COMPUTER );
        doAttack( PLAYER_HUMAN );
        system("clear");
        counter++;
    }
    return 0;
}
