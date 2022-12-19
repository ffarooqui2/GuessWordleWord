//  wordleGuessSecretWord.c
//  Have the program do the guessing to discover the secret wordle word.
//  UIC CS 211 Program 4
//  Author: Faaizuddin Farooqui
//
//  Links to wordle dictionary words at:
//    https://www.reddit.com/r/wordle/comments/s4tcw8/a_note_on_wordles_word_list/

#include <assert.h> // for assert() sanity checks
#include <ctype.h>  // for toupper()
#include <stdbool.h>
#include <stdio.h>  // for printf(), scanf()
#include <stdlib.h> // for exit( -1)
#include <string.h> // for strcpy
#include <time.h>   // for time()

// Declare globals
#define WORD_LENGTH 5
#define WORDS_FILE_NAME "wordsLarge.txt"
//#define WORDS_FILE_NAME "wordsTiny.txt"
#define MAX_NUMBER_OF_WORDS                                                    \
  12947         // Number of words in the full set of words file
#define true 1  // Make boolean logic easier to understand
#define false 0 // Make boolean logic easier to understand
#define undecided 2

typedef struct wordCount wordCountStruct;
struct wordCount {
  char word[WORD_LENGTH + 1]; // The word length plus the NULL character
  int score;                  // Score for the word
  int validWord;              // Acts as a boolean to check if the word is valid
};

//-----------------------------------------------------------------------------------------
// Read in words from file into array.  We've previously read the data file once
// to find out how many words are in the file.
void readWordsFromFile(
    char fileName[], // Filename we'll read from
    wordCountStruct
        *words,     // Array of words where we'll store words we read from file
    int *wordCount) // How many words.  Gets updated here and returned
{
  FILE *inFilePtr = fopen(fileName, "r"); // Connect logical name to filename
  assert(inFilePtr != NULL);              // Ensure file open worked correctly

  // Read each word from file and store into array, initializing the score for
  // that word to 0.
  char inputString[6];
  *wordCount = 0;
  while (fscanf(inFilePtr, "%s", inputString) != EOF) {
    strcpy(words[*wordCount].word, inputString);
    words[*wordCount].score = 0;
    (*wordCount)++;
  }

  // Close the file
  fclose(inFilePtr);
} // end readWordsFromFile(..)


//-----------------------------------------------------------------------------------------
// Comparator for use in built-in qsort(..) function.  Parameters are declared
// to be a generic type, so they will match with anything. This is a two-part
// comparison.  First the scores are compared.  If they are the same, then the
// words themselves are also compared, so that the results are in descending
// order by score, and within score they are in alphabetic order.
int compareFunction(const void *a, const void *b) {
  // Before using parameters we have cast them into the actual type they are in
  // our program and then extract the numerical value used in comparison
  int firstScore = ((wordCountStruct *)a)->validWord;
  int secondScore = ((wordCountStruct *)b)->validWord;

  // If scores are different, then that's all we need for our comparison.
  if (firstScore != secondScore) {
    // We reverse the values, so the result is in descending vs. the otherwise
    // ascending order return firstScore - secondScore;   // ascending order
    return secondScore - firstScore; // descending order
  } else {
    // Scores are equal, so check words themselves, to put them in alphabetical
    // order
    return strcmp(((wordCountStruct *)a)->word, ((wordCountStruct *)b)->word);
  }
} // end compareFunction(..)

int compareScoreFunction(const void *a, const void *b) {
  // Before using parameters we have cast them into the actual type they are in
  // our program and then extract the numerical value used in comparison
  int firstScore = ((wordCountStruct *)a)->score;
  int secondScore = ((wordCountStruct *)b)->score;

  // If scores are different, then that's all we need for our comparison.
  if (firstScore != secondScore) {
    // We reverse the values, so the result is in descending vs. the otherwise
    // ascending order return firstScore - secondScore;   // ascending order
    return secondScore - firstScore; // descending order
  } else {
    // Scores are equal, so check words themselves, to put them in alphabetical
    // order
    return strcmp(((wordCountStruct *)a)->word, ((wordCountStruct *)b)->word);
  }
} // end compareFunction(..)
// -----------------------------------------------------------------------------------------

// Removes a word from a list of wordCount structs
void removeWordFromList(wordCountStruct *allWords[], int *listSize, int index) {

  for (int i = index; i < (*listSize) - 1; i++) {
    (*allWords)[i] = (*allWords)[i + 1];
  }
  (*listSize) = (*listSize) - 1; // Adjusts the size of allWords
} // end revoveWordFromList(..)

int calculateSingleScore(char guessWord[], char answerWord[])
{
    int score = 0;

    // Compares words in search of exact position character matches 
    for(int i = 0; i < WORD_LENGTH; i++)
    {
        // Variables for legibility: Why does using these variables change the score?
        // char answerLetter = answerWord[i];
        // char guessLetter = guessWord[i];
      
        if(guessWord[i] == answerWord[i])
        {
            answerWord[i] = '*';   // replaces matched answer letter with a symbol to avoid re-matches
            guessWord[i] = '_';    // replaces matched guess letter with a symbol to avoid re-checks
            score += 3;            // increments score by 3 for an exact match
        }
    }

    // Compares words in search of offset position character matches
    for(int i = 0; i < WORD_LENGTH; i++)
    {
        // Variables for legibility
        // char guessLetter = guessWord[i];
      
        // Pointer to letter that exists within the answer, if non-NULL is an offset match
        char *pMatchLetter = NULL;
        pMatchLetter = strchr(answerWord, guessWord[i]);
      
        if(pMatchLetter != NULL)
        {               
            (*pMatchLetter) = '*';  // replaces matched letter with a symbol to avoid re-matches
            score += 1;             // increments score by 1 for an offset match
        }
    }
  return score;
} //end calculateSingleScore(...)

char *getBestWord(wordCountStruct * allWords[], int * listSize) {

  char *bestWord = malloc(sizeof(char) * 6);

  for (int i = 0; i < *listSize; i++){
    int score = 0;
    
    for (int j = 0; j < *listSize; j++){
      char guessWord[WORD_LENGTH + 1];
      char answerWord[WORD_LENGTH + 1];
      strcpy(guessWord, (*allWords)[i].word);
      strcpy(answerWord, (*allWords)[j].word);

      score += calculateSingleScore(guessWord, answerWord);
      
    }
    (*allWords)[i].score = score;
  }


  qsort(*allWords, *listSize, sizeof(wordCountStruct), compareScoreFunction);

  // Debugging Information:
  // printf("Best first word:\n");
  // printf("  %s %d\n", (*allWords)[0].word, (*allWords)[0].score);

  strcpy(bestWord, (*allWords)[0].word);

  return bestWord;
}
// -----------------------------------------------------------------------------------------
// Displays the word and it's prescribed feedback
void printFeedBack(char **computerGuess, char **secretWord, int *guessNumber,
                   wordCountStruct *allWords[], int indexOfWord) {

  char displayCharacter; // Used to display characters underneith word

  char copyOfComputerGuess[6];
  strcpy(copyOfComputerGuess, (*computerGuess));

  char copyOfSecretWord[6];
  strcpy(copyOfSecretWord, (*secretWord));

  printf("%5d. ", (*guessNumber));

  // Give feedback to user

  // CAPITALIZATION OF PERFECT MATCHES
  for (int i = 0; i < WORD_LENGTH; i++) {
    if (copyOfComputerGuess[i] != copyOfSecretWord[i]) // Not an exact match
      printf("%c ", copyOfComputerGuess[i]);

    else {
      printf("%c ", toupper(copyOfComputerGuess[i]));
      copyOfComputerGuess[i] = '*'; // replaces matched answer letter with a
                                    // symbol to avoid re-matches
      copyOfSecretWord[i] =
          '_'; // replaces matched guess letter with a symbol to avoid re-checks
    }
  }

  printf("\n");
  printf("       ");

  // NON-PERFECT MATCHES NOTATION
  for (int i = 0; i < WORD_LENGTH; i++) {
    displayCharacter = ' ';
    for (int j = 0; j < WORD_LENGTH; j++) {
      if (copyOfComputerGuess[i] == copyOfSecretWord[j]) {
        copyOfSecretWord[j] =
            '_'; // replaces matched letter with a symbol to avoid re-matches
        displayCharacter =
            '*'; // sets display character to * to indicate presence
      }
    }
    printf("%c ", displayCharacter);
  }
  printf("\n");
}

// -----------------------------------------------------------------------------------------
// Returns true or false depending if a list of letters are in a given word
int lettersInWord(char letters[], char word[]) {

  for (int i = 0; i < WORD_LENGTH; i++) {
    char *pointer = NULL;
    pointer = strchr(word, letters[i]);
    if ((pointer == NULL) && (letters[i] != '\0')) {
      return false;
    }
  }
  return true;
} // end lettersInWord(..)

// -----------------------------------------------------------------------------------------
// Find a secret word
void findSecretWord(wordCountStruct allWords[], // Array of all the words
                    int wordCount,     // How many words there are in allWords
                    char secretWord[]) // The word to be guessed
{

  int listLength = wordCount;

  char *computerGuess =
      malloc(sizeof(char) * 6); // Allocate space for the computer guess

  printf("Trying to find secret word: \n");
  // Display secret word with a space between letters, to match the guess words
  // below.
  printf("       ");
  for (int i = 0; i < WORD_LENGTH; i++) {
    printf("%c ", secretWord[i]);
  }
  printf("\n");
  printf("\n");

  int guessNumber = 1;

  for (int i = 0; i < listLength; i++) {
    allWords[i].validWord = undecided;
  }

  // Stores exact letters (their corrrect positions)
  char missingExactLetters[] = {' ', ' ', ' ', ' ', ' '};

  // Stores non exact letters
  char missingNonExactLetters[5];

  int currentNumberOfElements = 0;

  // Loop until the word is found

  while (true) {

    //int randomIndex;
    
    int indexOfBestWord;

    // First 5 guesses are hard coded values that give substantial information
    // to the computer.
    switch (guessNumber) {
    case 1:
      strcpy(computerGuess, "bling");
      break;
    case 2:
      strcpy(computerGuess, "jumpy");
      break;
    case 3:
      strcpy(computerGuess, "treck");
      break;
    case 4:
      strcpy(computerGuess, "vozhd");
      break;
    case 5:
      strcpy(computerGuess, "waqfs");
      break;
    case 6:
      strcpy(computerGuess, "feast");
      break;
    default:
      //Pick a random word from list (filtered)
      // randomIndex = (rand() % listLength);
      // strcpy(computerGuess, allWords[randomIndex].word);

      //Pick the best first word from the list of allWords (filtered after each new guess)
      // The indexOfBestWord is automatically zero because getBestWord sorts the list based on score

      computerGuess = getBestWord(&allWords, &listLength);

      indexOfBestWord = 0;
    }

    // Check if the computer guess is correct
    if (strcmp(computerGuess, secretWord) == 0) {
      printf("%5d. ", guessNumber);
      for (int i = 0; i < WORD_LENGTH; i++)
        printf("%c ", toupper(computerGuess[i]));
      printf("\n\n");
      printf("Got it!\n");
      break;
    }

    // Display feedback on letter guesses accurately
    printFeedBack(&computerGuess, &secretWord, &guessNumber, &allWords,
                  indexOfBestWord);

    char copyOfComputerGuess[6];
    char copyOfSecretWord[6];

    strcpy(copyOfComputerGuess, computerGuess);
    strcpy(copyOfSecretWord, secretWord);

    
    //Debugging Information: 
    // printf("BEFORE: \n");
    // for (int i = 0; i < listLength; i++){
    //   printf("%s %d %d\n", allWords[i].word, allWords[i].score, allWords[i].validWord);
    // }
    // printf("\n\n");

    // FILTER OUR LIST

    // Filter exact matches
    for (int i = 0; i < WORD_LENGTH; i++) {
      for (int j = 0; j < WORD_LENGTH; j++) {
        if (copyOfComputerGuess[i] == copyOfSecretWord[j] &&
            (currentNumberOfElements < 5)) {
          missingNonExactLetters[currentNumberOfElements] = secretWord[j];
          currentNumberOfElements++;
          copyOfSecretWord[j] =
              '_'; // replaces matched letter with a symbol to avoid re-matches
        }
      }
    }

    int size = 0; // keeps track of how many exact letters were found
    for (int i = 0; i < WORD_LENGTH; i++) {
      if (computerGuess[i] == secretWord[i]) {
        missingExactLetters[i] = secretWord[i];
        size++;
      }
    }

    // Traverse whole allWords array, evaluating each perfectly matched letters
    for (int i = 0; i < listLength; i++) {
      for (int j = 0; j < WORD_LENGTH; j++) {
        if (missingExactLetters[j] != ' ') {
          if (allWords[i].word[j] == missingExactLetters[j]) {
            allWords[i].validWord = true;
          } else {
            allWords[i].validWord = false;
            break;
          }
        }
      }
    }

    // Traverse whole allWords array, evaluating each imperfectly matched
    // letters
    for (int i = 0; i < listLength; i++) {
      if (lettersInWord(missingNonExactLetters, allWords[i].word) == true) {
        // Checking if the word was false beforehand
        if (allWords[i].validWord != false) {
          allWords[i].validWord = true;
        }
      } else {
        allWords[i].validWord = false;
      }
    }

    printf("\n\n");

    // IMPORTANT: the value should be adjusted
    // Ensures the chosen word, though it may be valid, must be removed
    if (guessNumber > 6) {
      allWords[indexOfBestWord].validWord = false;
    }

    // Traverses allWords array, removing words if they are invalid
    for (int i = 0; i < listLength; i++) {
      // If exact matches were found, then any undecided words must also be
      // removed
      if (size != 0) {
        if (allWords[i].validWord == false ||
            allWords[i].validWord == undecided) {
          removeWordFromList(&allWords, &listLength, i);
          i--;
        }
      } else {
        if (allWords[i].validWord == false) {
          removeWordFromList(&allWords, &listLength, i);
          i--;
        }
      }
    }

    //Debugging Information: 
    // printf("AFTER: \n");
    // for (int i = 0; i < listLength; i++){
    //   printf("%s %d %d\n", allWords[i].word, allWords[i].score, allWords[i].validWord);
    // }
    
    
    // Update guess number
    guessNumber++;
  } // end for( int i...)
} // end findSecretWord

// -----------------------------------------------------------------------------------------
int main() {
  char wordsFileName[81]; // Stores the answers file name
  strcpy(wordsFileName,
         WORDS_FILE_NAME); // Set the filename, defined at top of program.
  srand((unsigned)time(
      NULL)); // Seed the random number generator to be current time
  // Declare space for all the words, of a maximum known size.
  wordCountStruct allWords[MAX_NUMBER_OF_WORDS];
  // Start out the wordCount to be the full number of words.  This will decrease
  // as
  //    play progresses each time through the game.
  int wordCount = 0;
  // The secret word that the computer will try to find, plus the return
  // character from fgets.
  char secretWord[WORD_LENGTH + 1];
  char userInput[81]; // Used for menu input of secret word

  // Read in words from file, update wordCount and display information
  readWordsFromFile(wordsFileName, allWords, &wordCount);
  printf("Using file %s with %d words. \n", wordsFileName, wordCount);

  // Run the word-guessing game three times
  for (int i = 0; i < 3; i++) {

    readWordsFromFile(wordsFileName, allWords, &wordCount);
    // Reset secret Word
    strcpy(secretWord, "");
    // Display prompt
    printf("-----------------------------------------------------------\n");
    printf("\n");
    printf("Enter a secret word or just r to choose one at random: ");
    scanf(" %s", userInput);
    // Eliminate the return character at end or userInput if it is there
    int length = (int)strlen(userInput);
    if (userInput[length] == '\n') {
      userInput[length] = '\0';
    }
    strcpy(secretWord, userInput); // Store the secret word from user input

    // If input was 'r' then choose a word at random.
    if (strlen(secretWord) <= 1) {
      // Randomly select a secret word to be guessed.
      int randomIndex = rand() % wordCount;
      strcpy(secretWord, allWords[randomIndex].word);
    }
    // Run the game once with the current secret word
    // for (int j = 0 ; j < wordCount; j++){
    //   printf("%s \n",allWords[j].word);
    // }
    findSecretWord(allWords, wordCount, secretWord);
  }
  
  printf("Done\n");
  printf("\n");
  return 0;
} // end main()
