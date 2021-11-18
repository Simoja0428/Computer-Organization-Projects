#include <stdio.h> //Important in all C programs
int MyStrlen(char * s) //Function returns length of strings in command line
{
  int retVal = 0; //retVal is used as a tracker for the size of the input
  mtop: //Top label used in place of a loop
      if ( *s == NULL) //The last index will be null, terminates function
          goto mbottom; //Moves to the bottom to return the function
      retVal = retVal + 1; //Adds to the tracker
      s = s + 1; //Adds to the address of the tracker
      goto mtop; //Goes back to the top reseting the loop variation
  mbottom: //Bottom label used to exit function
      return retVal; //Returns the trackeer
}
int main(int argc, char * argv[]) //Main function contains command parameters
{
  top: //Top label used in place of a loop
      if ( *argv == NULL) //The last index of argv will be Null, terminator condition
          goto bottom; //Moves to the bottom to return
      printf("[%d] [%s]\n", MyStrlen(*argv), *argv ); //Prints contents
      argv = argv + 1; //Increments argv's address NOT THE VALUE
      goto top; //Goes back to the top acting kind of like a look
  bottom: //Bottom label is used to exit the program
      return 0; //Exits the program
}
