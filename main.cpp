/*******************************************************************************************************************
	Program: DNA Profiling
	Description: This program reads in DNA files and databases in order to match people's 
	DNA based on their STR (short-tandem repeats) counts.
	Class: CS 251, Wednesday 10am lab
	System used: Replit
	Author: Arwa Mazher

	CREATIVE COMPONENT: My creative component counts how much each nucleotide appears
			given the DNA file. In order to run this, use command count_nucleotides
			AFTER a database has been loaded (load_db) and DNA has been loaded (load_dna).

		Sample Input/Output (integers are in order of A T G C):
		load_db small.txt, load_dna 1.txt, count_nucleotides --> 75, 46, 40, 9 (max: 75, min: 9, average: 42)
		load_db small.txt, load_dna 2.txt, count_nucleotides --> 78, 69, 74, 79 (max: 79, min: 69, average: 75)
		load_db small.txt, load_dna 3.txt, count_nucleotides --> 113, 65, 55, 8
		load_db large.txt, load_dna 15.txt, count_nucleotides --> 1571, 1528, 1331, 1327
		load_db large.txt, load_dna 16.txt, count_nucleotides --> 1585, 1556, 1383, 1355
		load_db large.txt, load_dna 17.txt, count_nucleotides --> 1425, 1749, 1296, 1277
		
*******************************************************************************************************************/
#include "ourvector.h"
#include <fstream>
#include <sstream>
using namespace std;

// For loading database from files
struct Person {
  string name;
  ourvector<int> numSequences;        // Number of mentioned STR the person has
  ourvector<ourvector<char>> fileSTR; // For storing STR as chars
};

// Read in database files
void loadDatabase(ourvector<Person> &database, string fileName, string &dbFile);

// Display database files
void displayDatabase(ourvector<Person> &database);

// Read in DNA files
void loadDNA(ourvector<char> &DNA, string fileName, string &dnaFile);

// Display DNA
void displayDNA(ourvector<char> &DNA);

// Process DNA
void processDNA(ourvector<char> &DNA, ourvector<Person> &database, ourvector<int> &longestStreaks);

// To find the longest occuring number of STR
int findMaxStreak(ourvector<int> &streakNums);

// Display processed DNA
void displayProcess(ourvector<Person> &database, ourvector<int> &longestStreaks);

// Search the database
void searchDatabase(ourvector<Person> &database, ourvector<int> &longestStreaks);

// Creative component - count and display stats for all nucleotides
void countNucleotides(ourvector<char> &DNA);
void displayNucleoCounts(int countA, int countT, int countG, int countC);

/*********************************************************************************/

int main() {
  string command, fileName, dbFile, dnaFile;
  ourvector<Person> database;
  ourvector<char> DNA;
	ourvector<int> longestStreaks;
  bool isDatabaseLoaded = false;
  bool isDNALoaded = false;
  bool isDNAProcessed = false;

  cout << "Welcome to the DNA Profiling Application." << endl;

  while (true) {
    cout << "Enter command or # to exit: ";
    getline(cin, command);

    // Find index of space between command and file name and then assign
    // variables accordingly
    int spaceIndex = command.find(' ');
    if (spaceIndex != -1)
      fileName = command.substr(spaceIndex + 1);
    command = command.substr(0, spaceIndex);

    if (command == "#")
      break;

    if (command == "load_db") {
      cout << "Loading database..." << endl;
      if ((fileName != "small.txt") && (fileName != "large.txt")) {
        cout << "Error: unable to open '" << fileName << "'" << endl;
        continue;
      }
      loadDatabase(database, fileName, dbFile);
      isDatabaseLoaded = true;
    } // end load_db command

    if (command == "display") {
      if (!isDatabaseLoaded)
        cout << "No database loaded." << endl;
      else
        cout << "Database loaded: " << endl;

      displayDatabase(database);

      if (!isDNALoaded)
        cout << "No DNA loaded.";
      else
        cout << "DNA loaded: " << endl;

      displayDNA(DNA);

      if (!isDNAProcessed)
        cout << "No DNA has been processed." << endl;
			else
				displayProcess(database, longestStreaks);
    } // end display command

    if (command == "load_dna") {
      cout << "Loading DNA..." << endl;
			// Use a stringstream to extract just the integer value of the DNA file
      int numFile;
      stringstream s(fileName);
      s >> numFile;

      // Check if valid file number
      if ((numFile < 1) || (numFile > 20)) {
        cout << "Error: unable to open '" << fileName << "'" << endl;
        continue;
      }
      loadDNA(DNA, fileName, dnaFile);
      isDNALoaded = true;
    } // end load_dna command

    if (command == "process") {
			if (!isDatabaseLoaded) {
				cout << "No database loaded." << endl;
				continue;
			}
			if (!isDNALoaded) {
				cout << "No DNA loaded." << endl;
				continue;
			}
      cout << "Processing DNA..." << endl;
      processDNA(DNA, database, longestStreaks);
			isDNAProcessed = true;
    } // end process command

    if (command == "search") {
			if (!isDatabaseLoaded) {
				cout << "No database loaded." << endl;
				continue;
			}
			if (!isDNALoaded) {
				cout << "No DNA loaded." << endl;
				continue;
			}
			if (!isDNAProcessed) {
				cout << "No DNA processed." << endl;
				continue;
			}
			cout << "Searching database..." << endl;
			searchDatabase(database, longestStreaks);
    } // end search command

		// Creative component
		if (command == "count_nucleotides") {
			if (!isDatabaseLoaded) {
				cout << "No database loaded." << endl;
				continue;
			}
			if (!isDNALoaded) {
				cout << "No DNA loaded." << endl;
				continue;
			}
			cout << "Counting nucleotides..." << endl;
			countNucleotides(DNA);
		} // end search_nucleotides command

  } // end while

} // end main

/*********************************************************************************/
void loadDatabase(ourvector<Person> &database, string fileName, string &dbFile) {
  dbFile = fileName;
  Person p1;
  ifstream inFS;
  string line;

  database.clear();
  inFS.open(dbFile);

  // Store STRs in the first line
  getline(inFS, line, '\n');
  int commaIndex = line.find(',');
  string sequence = line.substr(commaIndex + 1); // excludes 'name,' and only has the nucleotides
  stringstream ss(sequence);
  string token;

  while (getline(ss, token, ',')) {
    ourvector<char> STRs;
    char c;
    for (int i = 0; i < token.size(); i++) {
      c = token.at(i);
      STRs.push_back(c);
    }
    p1.fileSTR.push_back(STRs);
  }

  // Store rest of the file data
  while (!inFS.eof()) {
    getline(inFS, line, '\n'); // repeat getline() to skip first line
    int commaIndex = line.find(',');
    p1.numSequences.clear();
    p1.name = line.substr(0, commaIndex);

    string temp = line.substr(commaIndex + 1); // string with only the numbers
    // Parse through temp string and add numbers to numSequences ourvector
    stringstream s(temp);
    while (s.good() && (temp.size() > 0)) {
      string substr;
      int tempNum;
      s >> tempNum;
      getline(s, substr, ',');
      // cout << substr << endl;
      p1.numSequences.push_back(tempNum);
    }
    database.push_back(p1);
  }
  inFS.close();
} // end loadDatabase()

/*********************************************************************************/
void displayDatabase(ourvector<Person> &database) {
  for (Person p1 : database) {
    cout << p1.name << " ";
    for (int i = 0; i < p1.numSequences.size(); i++) {
      cout << p1.numSequences.at(i) << " ";
    }
    cout << endl;
  }
} // end displayDatabase()

/*********************************************************************************/
void loadDNA(ourvector<char> &DNA, string fileName, string &dnaFile) {
  dnaFile = fileName;
  ifstream inFS;
  string token;

  DNA.clear();
  inFS.open(dnaFile);
  getline(inFS, token);

  for (char c : token) {
    DNA.push_back(c);
  }

  inFS.close();
} // end loadDNA()

/*********************************************************************************/
void displayDNA(ourvector<char> &DNA) {
  for (char c : DNA) {
    cout << c;
  }
  cout << endl << endl;
} // end displayDNA()

/*********************************************************************************/
void processDNA(ourvector<char> &DNA, ourvector<Person> &database, ourvector<int> &longestStreaks) {
  int streak = 0; // keep track of longest sequence
  ourvector<int> streakNums;
  ourvector<char> STR;

	// Loop through all the STRs
  for (int i = 0; i < database.at(0).fileSTR.size(); i++) {
    STR = database.at(0).fileSTR.at(i);	// Current STR for comparison e.g. AGATC, AATG, etc.
    streakNums.clear();
    int posDNA = 0;
    int posSTR = 0;
		int holdIndex = 0;

    while (posDNA < DNA.size()) {
			// Reset the STR to the beginning so you can start comparing again
      if (posSTR == STR.size()) {
        posSTR = 0;
      }

			// If the DNA and STR match, move forward and add one to the streak
      if (DNA.at(posDNA) == STR.at(posSTR)) {
        posDNA++;
        posSTR++;
        streak++;
      }

			// If they don't match, reset the STR position and go back to the first correct letter + 1 (to compare the next letter)
      else {
        posSTR = 0; // Start comparing from start of STR
				holdIndex++;
        posDNA = holdIndex;

				// Add the streak to streakNums so you have a record of how many STRs in a row you found
				// Then reset the streak
        streakNums.push_back(streak / STR.size());
        streak = 0;
      }
    } // end while

		// Find the longest streak from all the streaks recorded in streakNums and push it back to the vector longestStreaks
    longestStreaks.push_back(findMaxStreak(streakNums));

  } // end for loop

} // end processDNA()

/*********************************************************************************/
int findMaxStreak(ourvector<int> &streakNums) {
  int max = streakNums.at(0);
  for (int x : streakNums) {
    if (x > max)
      max = x;
  }
  return max;
} // end findMaxStreak()

/*********************************************************************************/
void displayProcess(ourvector<Person> &database, ourvector<int> &longestStreaks) {
	ourvector<char> STR;	// Vector with just the STR in it (for ease of display)
	cout << "DNA processed, STR counts:" << endl;
  for (int i = 0; i < database.at(0).fileSTR.size(); i++) {
		STR = database.at(0).fileSTR.at(i);

		for (char c : STR) {
			cout << c;
		}

		cout << ": " << longestStreaks.at(i) << endl;
		
  }
} // end displayProcess()

/*********************************************************************************/
void searchDatabase(ourvector<Person> &database, ourvector<int> &longestStreaks) {
	bool foundMatch = false;
	int numMatches = 0;
	string matchName;
	ourvector<int> STRcounts;	// Vector with only the integer values of STR counts

	for (int i = 0; i < database.size(); i++) {
		numMatches = 0;
		STRcounts = database.at(i).numSequences;
		for (int j = 0; j < STRcounts.size(); j++) {
			if (STRcounts.at(j) == longestStreaks.at(j)) {
				numMatches++;
				foundMatch = true;
			}
			if (numMatches == STRcounts.size()) {
				matchName = database.at(i).name;
				break;
			}
		}
	}

	if (foundMatch && (matchName != ""))
		cout << "Found in database! DNA matches: " << matchName << endl;
	else
		cout << "Not found in database." << endl;
} // end searchDatabase()

/*********************************************************************************/
// Creative component
void countNucleotides(ourvector<char> &DNA) {
	// DNA consists of four nucleotides: A, T, G, C
	int countA = 0;
	int countT = 0;
	int countG = 0;
	int countC = 0;

	for (char c : DNA) {
		if (c == 'A')
			countA++;
		else if (c == 'T')
			countT++;
		else if (c == 'G')
			countG++;
		else
			countC++;
	}
	displayNucleoCounts(countA, countT, countG, countC);
} // end countNucleotides()

/*********************************************************************************/
// Creative component
void displayNucleoCounts(int countA, int countT, int countG, int countC) {
	ourvector<int> nucleoCounts;	// Create a vector so that it's easier to find max, min counts
	nucleoCounts.push_back(countA);
	nucleoCounts.push_back(countT);
	nucleoCounts.push_back(countG);
	nucleoCounts.push_back(countC);

	// Find max count
	int max = 0;
	for (int x : nucleoCounts) {
		if (x > max) {
			max = x;
		}
	}

	// Find min count
	int min = nucleoCounts.at(0);
	for (int x : nucleoCounts) {
		if (x < min) {
			min = x;
		}
	}
	
	cout << "Nucleotides counted. Stats: " << endl;
	cout << "A count: " << countA << endl;
	cout << "T count: " << countT << endl;
	cout << "G count: " << countG << endl;
	cout << "C count: " << countC << endl;
	cout << "Total counts: " << countA + countT + countG + countC << endl;
	cout << "Max count: " << max << endl;
	cout << "Min count: " << min << endl;
	cout << "Average nucleotide count: " << (countA + countT + countG + countC) / 4 << endl;
	// NOTE: I left the average as an integer since you can't have part of a nucleotide
	
} // end displayNucleoCounts()