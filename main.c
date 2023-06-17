#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <errno.h>

/* struct for movie information */
struct movie
{
    char *title;
    char *year;
    char *language;
    char *rating;
    struct movie *next;
};
//void freeList(struct movie *list);

void displayOptions(int choice);

/* Parse the current line which is space delimited and create a
*  movie struct with the data in this line
*/
struct movie *createMovie(char *currLine)
{
    struct movie *currMovie = malloc(sizeof(struct movie));

    // For use with strtok_r
    char *saveptr;

    // The first token is the title
    char *token = strtok_r(currLine, ",", &saveptr);
    currMovie->title = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->title, token);

    // The next token is the year
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->year = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->year, token);

    // The next token is the language
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->language = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->language, token);

    // The last token is the rating
    token = strtok_r(NULL, ",", &saveptr);
    currMovie->rating = calloc(strlen(token) + 1, sizeof(char));
    strcpy(currMovie->rating, token);

    // Set the next node to NULL in the newly created movie entry
    currMovie->next = NULL;

    return currMovie;
}

/*
* Return a linked list of movies by parsing data from
* each line of the specified file.
*/


struct movie *processFile(char *filePath)
{
    // Open the specified file for reading only
    FILE *movieFile = fopen(filePath, "r");

    char *currLine = NULL;
    size_t len = 0;
    ssize_t nread;
    char *token;

    // The head of the linked list
    struct movie *head = NULL;
    // The tail of the linked list
    struct movie *tail = NULL;
    // Flag to indicate whether the current line is the first line
    int firstLine = 1;
    // Read the file line by line
    while ((nread = getline(&currLine, &len, movieFile)) != -1)
    {
        // Skip the first line
        if (firstLine)
        {
            firstLine = 0;
            continue;
        }
        // Get a new movie node corresponding to the current line
        struct movie *newNode=createMovie(currLine);

        // Is this the first node in the linked list?
        if (head == NULL)
        {
            // This is the first node in the linked link
            // Set the head and the tail to this node
            head = newNode;
            tail = newNode;
        }
        else
        {
            // This is not the first node.
            // Add this node to the list and advance the tail
            tail->next = newNode;
            tail = newNode;
        }
    }
    free(currLine);
    fclose(movieFile);
    return head;
}

void printMovie(struct movie* aMovie){
  printf("%s, %s %s, %s\n", aMovie->title,
               aMovie->year,
               aMovie->language,
               aMovie->rating);
}

void printMovieList(struct movie *list)
{
    while (list != NULL)
    {
        printMovie(list);
        list = list->next;
    }
}

void showMoviesInYear(struct movie *list)
{
    int year;

    printf("Enter the year for which you want to see movies: ");
    scanf("%d", &year);

    struct movie *current = list;
    int found = 0;

    while (current != NULL)
    {
        if (atoi(current->year) == year)
        {
            printf("%s\n", current->title);
            found = 1;
        }
        current = current->next;
    }

    if (!found)
    {
        printf("No movies found for the year %d.\n", year);
    }
}

void showHighestRatedMovies(struct movie *list) {
    // Create a temporary pointer to the head of the linked list
    struct movie *temp = list;

    // Create a map to store the highest rated movie for each year
    int year;
    float rating;
    char title[100];
    struct movie *highestRatedMovies[2022-1900];  // Map for years from 1900 to 2021
    for (int i = 0; i < 2022-1900; i++) {
        highestRatedMovies[i] = NULL;
    }

    // Iterate through the linked list and update the map with highest rated movies
    while (temp != NULL) {
        year = atoi(temp->year);
        rating = strtof(temp->rating, NULL);
        if (highestRatedMovies[year-1900] == NULL || strtof(highestRatedMovies[year-1900]->rating, NULL) < rating) {
            highestRatedMovies[year-1900] = temp;
        }
        temp = temp->next;
    }

    // Print the highest rated movie for each year
    for (int i = 0; i < 2022-1900; i++) {
        if (highestRatedMovies[i] != NULL) {
            printf("%d %.1f %s\n", i+1900, strtof(highestRatedMovies[i]->rating, NULL), highestRatedMovies[i]->title);
        }
    }
}

void showMoviesByLanguage(struct movie *list) {
    char language[100];
    printf("Enter a language: ");
    scanf("%99s", language);
    language[strcspn(language, "\n")] = '\0'; // Remove newline character

    struct movie *temp = list;
    int found = 0;

    // Loop through the linked list
    while (temp != NULL) {
        // Copy the language string to a temporary buffer for parsing
        char lang[100];
        strcpy(lang, temp->language);

        // Parse the language string into individual languages
        char *token = strtok(lang, "[];");
        while (token != NULL) {
            // Check if the current language matches the requested language
            if (strcmp(token, language) == 0) {
                // Print the movie information
                printf("%s (%s) %s\n", temp->title, temp->year, temp->rating);
                found = 1;
                break;
            }
            token = strtok(NULL, "[];");
        }
        temp = temp->next;
    }
    if (!found) {
        printf("No movies found for the given language.\n");
    }
}

void processLargestFile() {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    char *largestFile = NULL;
    off_t largestSize = -1;

    // Open current directory
    dir = opendir(".");
    if (dir == NULL) {
        fprintf(stderr, "Error opening current directory.\n");
        return;
    }

    // Traverse directory and find largest csv file starting with movies_
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strncmp(entry->d_name, "movies_", 7) == 0 && strstr(entry->d_name, ".csv") != NULL) {
            stat(entry->d_name, &fileStat);
            if (fileStat.st_size > largestSize) {
                largestSize = fileStat.st_size;
                largestFile = entry->d_name;
            }
        }
    }

    printf("Now processing the file %s\n", largestFile);

    if (largestFile == NULL) {
        printf("No movie files found in current directory.\n");
        return;
    }

    // Process largest file
    struct movie *list = processFile(largestFile);

    // Create directory
    char *onid = "chongjoo";
    char dirname[PATH_MAX];
    srand(time(NULL));
    int random = rand() % 100000;
    snprintf(dirname, PATH_MAX, "%s.movies.%d", onid, random);
    if (mkdir(dirname, S_IRWXU | S_IRGRP | S_IXGRP) == -1) {
        fprintf(stderr, "Error creating directory %s\n", dirname);
        return;
    }
    printf("Directory %s has been created.\n", dirname);

    // Parse data to find movies released in each year and create files
    struct movie *cur = list;
    while (cur != NULL) {
        char *year = cur->year;
        char filename[PATH_MAX];
        snprintf(filename, PATH_MAX, "./%s/%s.txt", dirname, year);
        FILE *fp = fopen(filename, "a");
        if (fp == NULL) {
            fprintf(stderr, "Error creating file %s\n", filename);
            return;
        }
        fprintf(fp, "%s\n", cur->title);
        fclose(fp);
        cur = cur->next;
    }

    // Clean up
    //freeList(list);
    closedir(dir);
}

void processSmallestFile() {
    DIR *dir;
    struct dirent *entry;
    struct stat fileStat;
    char *smallestFile = NULL;
    off_t smallestSize = -1;

    // Open current directory
    dir = opendir(".");
    if (dir == NULL) {
        fprintf(stderr, "Error opening current directory.\n");
        return;
    }

    // Traverse directory and find largest csv file starting with movies_
    while ((entry = readdir(dir)) != NULL) {
            if (strncmp("movies_", entry->d_name, 7) == 0 && strstr(entry->d_name, ".csv") != NULL) {
            stat(entry->d_name, &fileStat);
                if (smallestSize == -1 || fileStat.st_size < smallestSize) {
                    smallestSize = fileStat.st_size;
                    smallestFile = entry->d_name;
            }
        }
    }

    if (smallestFile == NULL) {
        printf("No movie files found in current directory.\n");
        return;
    }

    printf("Now processing the file %s\n", smallestFile);

    // Process largest file
    struct movie *list = processFile(smallestFile);

    // Create directory
    char *onid = "chongjoo";
    char dirname[PATH_MAX];
    srand(time(NULL));
    int random = rand() % 100000;
    snprintf(dirname, PATH_MAX, "%s.movies.%d", onid, random);
    if (mkdir(dirname, S_IRWXU | S_IRGRP | S_IXGRP) == -1) {
        fprintf(stderr, "Error creating directory %s\n", dirname);
        return;
    }
    printf("Directory %s has been created.\n", dirname);

    // Parse data to find movies released in each year and create files
    struct movie *cur = list;
    while (cur != NULL) {
        char *year = cur->year;
        char filename[PATH_MAX];
        snprintf(filename, PATH_MAX, "./%s/%s.txt", dirname, year);
        FILE *fp = fopen(filename, "a");
        if (fp == NULL) {
            fprintf(stderr, "Error creating file %s\n", filename);
            return;
        }
        fprintf(fp, "%s\n", cur->title);
        fclose(fp);
        cur = cur->next;
    }

    // Clean up
    //freeList(list);
    closedir(dir);
}

void processSpecifiedFile(char* filename)
{
    // check if file exists
    if (access(filename, F_OK) == -1) {
        printf("File not found. Please try again.\n");
        displayOptions(1);
        return;
    }

    printf("Now processing the file %s\n", filename);

    // create directory
    char dirname[100];
    sprintf(dirname, "%s.movies.%d", "chongjoo", rand() % 100000);
    mkdir(dirname, S_IRWXU | S_IRGRP | S_IXGRP);
    printf("Created directory: %s\n", dirname);

    // parse data in file to find out movies released in each year
    struct movie *list = processFile(filename);
    struct movie *temp = list;
    while (temp != NULL) {
        // get year from movie struct
        int year = atoi(temp->year);

        // create file for year if it doesn't exist
        char filename[100];
        sprintf(filename, "%s/%d.txt", dirname, year);
        FILE *file = fopen(filename, "a+");
        fclose(file);

        // write movie title to file
        file = fopen(filename, "a+");
        fprintf(file, "%s\n", temp->title);
        fclose(file);

        temp = temp->next;
    }

    // set file permissions
    struct stat st;
    stat(dirname, &st);
    chmod(dirname, (st.st_mode & S_IRWXU) | S_IRGRP | S_IXGRP);

    DIR *dir = opendir(dirname);
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        char filename[100];
        sprintf(filename, "%s/%s", dirname, ent->d_name);
        stat(filename, &st);
        chmod(filename, (st.st_mode & S_IRUSR) | S_IWUSR | S_IRGRP);
    }
    closedir(dir);
}

//function to display all 3 options 
void displayMenu() {
    printf("Please choose an option between 1-3\n");
    printf("Enter 1 to pick the largest file\n");
    printf("Enter 2 to pick the smallest file\n");
    printf("Enter 3 to specify the name of a file\n");
}

void displayOptions(int choice){
            //switch case to get userinput for which function they want to perform
        int fileChoice = 0;
        char lang[20];
        switch (choice) {
            case 1:
                while (1) {
                    printf("Which file do you want to process?\n");
                    displayMenu();
                    scanf("%d", &fileChoice);
                if (fileChoice >= 1 && fileChoice <= 3) {
                    break;
                }else {
                    printf("Invalid choice. Please enter a number between 1 and 3.\n");
                 }
    }

    if(fileChoice == 1){
        processLargestFile();
    }
    else if(fileChoice == 2){
        processSmallestFile();
    }
    else if(fileChoice == 3){
        printf("Please type the name of the file you want to search for\n");
        scanf("%s",&lang);
        processSpecifiedFile(lang);
    }
    break;
            case 2:
                exit(0);
                break;
            default:
                printf("Invalid choice. Please enter a number between 1 and 2.\n");
                break;
        }
    
}

int main(int argc, char *argv[]){
    if (argc < 2) {
        printf("You must provide the name of the file to process\n");
        printf("Example usage: ./movie movies_sample_1.csv\n");
        return EXIT_FAILURE;
    }
    struct movie *list = processFile(argv[1]);

//while loop to make sure user chooses option between 1-2
    while (1) {
        int choice = 0;
        int fileChoice = 0;
        char lang[20];

        printf("Enter your choice (1-2):\n");
        printf("1. Select file to process\n");
        printf("2. Exit the program\n");
        scanf("%d", &choice);
        displayOptions(choice);

/*
        //switch case to get userinput for which function they want to perform
        switch (choice) {
            case 1:
                printf("Which file do you want to process?\n");
                displayMenu();
                scanf("%d", &fileChoice);
                while (fileChoice < 1 || fileChoice > 3) {
                    printf("Invalid choice. Please enter a number between 1 and 3.\n");
                    displayMenu();
                    scanf("%d", &fileChoice);
                }

                if(fileChoice == 1){
                    processLargestFile();
                }
                else if(fileChoice == 2){
                    processSmallestFile();
                }
                else if(fileChoice == 3){
                    printf("Please type the name of the file you want to search for\n");
                    scanf("%s",&lang);
                    processSpecifiedFile(lang);
                }
                break;
            case 2:
                exit(0);
                break;
            default:
                printf("Invalid choice. Please enter a number between 1 and 2.\n");
                break;
        }
        */
    }

    return EXIT_SUCCESS;
}