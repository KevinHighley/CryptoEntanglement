/**********************************************************************/
/*                  cryptoentanglement.cxx             			      */
/*                                                                    */
/**********************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// This program frequently does the same thing to all 256 elements of
// an array.   The macro Ri means 'Repeat for all i'
#define Ri for(i=0;i<=255;i++)

/**********************************************************************/
// 'uint8_t' is the same as 'unsigned char'

uint8_t s[256];		// secret encrypted message transfered in here
uint8_t r[256];		// result decoded message ends up here
uint8_t o[256];		// original human readable message in here
uint8_t xs[256][6];	// Code wheels on source machine
uint8_t xd[256][6];	// Code wheels on destination machine
int key[6];			// User keys.
int	w[6];			// Wheel selectors


/**********************************************************************/
/*																	  */
/*	Generate the initial source and destination code wheels		      */
/* These must be the same if the message is to be correctly decoded.  */
/*																	  */
/**********************************************************************/

void makeWheels()
{uint8_t a,b,z;
 int i,j;
 
 printf("Enter a random number seed, to create the code blocks.\n");
 scanf("%d/n",&i);		
 srand(i);							// Seed random generator
 
 for(j=0;j<=5;j++)					// for each of the 6 code wheels
	{Ri xs[i][j] = i;               // Fill array with values 0 - 255
     for(i=0;i<30000;i++)			// Swap randomly chosen pairs of
        {a=rand();					// values 30000 times to give
         b=rand();					// the desired wheel, all possible
         z=xs[a][j];				// 8 bit numbers, but in an
         xs[a][j]=xs[b][j];			// unknown order.  Like Eric
         xs[b][j]=z;			    // Morecombe's music - "All the 
        }							// right notes, but not in the
        							// right order."
	 Ri xd[i][j] = xs[i][j];		// Copy to destination array
	}								// This would be the entangling
 return;							// machines step in working system.	
}

/**********************************************************************/
//
//	Keys would not be kept on the machines, they need to be secret!
//  For this demo thay are entered here, and used by both send and
//	receive code
//
/**********************************************************************/

void getKeys()
{key[0] = 76;  key[1] = 87; key[2] = 120; 
 key[3] = 200; key[4] = 55; key[5] = 99;
 w[0] = 1; w[1] = 3; w[2] = 2; w[3] = 0;
 w[4] = 4; w[5] = 5;
 return;
}  

/**********************************************************************/ 
/*																	  */
/*	Fill the origin array with text to act as source message.		  */
/*																	  */
/**********************************************************************/

void getmessage()
{int i; char c;
 printf("\n\nType message, ending with 'Return' start with 'Z' to exit.\n");	
 Ri o[i]='_';						// Fill message with underscore
 c = getchar();						// put characters from keyboard 
 Ri {if (!isprint(c)) break;		// into message until you get a non 
     o[i] = c;						// printable - probably <cr>
     c = getchar();
    }
 return;
}

/**********************************************************************/
//	Encrypt
//
//  First jumble the order of the message,
//  Next add the values of the first code wheel to the message array. 
//  Then the main encryption stage, similar to an enigma code. As a 
//	character in encoded by a wheel it also moves that wheel to a new
//  position. Repeat 5 times with five different wheels.
//	Finally scramble code wheels using the intermediate stages
//  of the above encryption to control the scramble.
//
//	The user keys determine the order of the wheels (w[ ]), and the 
//  position each one starts at (k[ ]).
//
/**********************************************************************/

void encrypt()
{int i,j;								// Loop counters
 uint8_t k;								// wheel position
 uint8_t z;								// temp for swaping values
 uint8_t a[256][6];						// Intermediate stages
	
 Ri a[i][0] = o[xs[i][w[0]]];			// jumble order of message
 
 k = key[0];    			
 Ri a[i][0] += xs[k++][w[0]];			// Add wheel w[0] offset K[0] 
 
 for(j=1;j<=5;j++)		 				// Main enigma loop	
	{k = key[j];
	 Ri {a[i][j] = a[i][j-1] + xs[k][w[j]];
		 k = xs[a[i][j-1]][w[j]];
	}	}
	
 for(j=0;j<=5;j++)						// Wheel scrambling
	{Ri {z = xs[i][w[j]];
	     xs[i][w[j]] = xs[a[i][j]][w[j]];
	     xs[a[i][j]][w[j]] = z;
	}   }
	
 Ri s[i] = a[i][5];						//Transmit encoded message			

 return;	
}

/**********************************************************************/
//
//	Decrypt
//
/**********************************************************************/

void decrypt()
{int i,j;
 uint8_t k;	
 uint8_t z;
 uint8_t a[256][7];
 
 Ri a[i][6] = s[i];					// receive message
 
 for(j=5;j>=1;j--)					// Undo enigma coding	 					
	{k = key[j];
	 Ri {a[i][j] = a[i][j+1] - xd[k][w[j]];
		 k = xd[a[i][j]][w[j]];
	}	}
	
 k = key[0];    					
 Ri a[i][0]=a[i][1]-xd[k++][w[0]];	// Subtract wheel w[0] offset K[0]	 

 Ri r[xd[i][w[0]]] = a[i][0];		// Undo order jumbling

 for(j=0;j<=5;j++)					// Scramble wheels to match
	{Ri {z = xd[i][w[j]];			// transmitter.
	     xd[i][w[j]] = xd[a[i][j+1]][w[j]];
	     xd[a[i][j+1]][w[j]] = z;
	}   }
	    			
 return;	
} 

/**********************************************************************/
//
//	Display message, source, encrypted and decrypted 
//
/**********************************************************************/

void showArrays()
{int i,j;

 printf("\n   Original Message\n");
 for(i=0;i<=255;i+=64)
	{for(j=0;j<64;j++)printf("%c",o[i+j]);
	 printf("\n");
	}
 printf("\n   Encrypted Message\n");
 for(i=0;i<=255;i+=64)	// use . for non printing characters
	{for(j=0;j<64;j++){if (!isprint(s[i+j]))printf(".");
		               else printf("%c",s[i+j]);}
	 printf("\n");
	}
 printf("\n   Decoded Message\n");
 for(i=0;i<=255;i+=64)
	{for(j=0;j<64;j++)printf("%c",r[i+j]);
	 printf("\n");
	}
 return;	
}

/**********************************************************************/

int main(int argc, char **argv)
{makeWheels();
 getKeys();
 while (o[0]!='Z') 
	{getmessage();
	 encrypt();
     decrypt();
     showArrays();
    }
 return 0;
}
