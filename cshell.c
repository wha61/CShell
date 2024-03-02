#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

// for environment variable
typedef struct {
	char name[200];
	char value[200];
} EnvVar;


//for the log command
typedef struct {
	char name[200];
	struct tm *time;
	char timeStirng[200];
	int value;
} Command;

Command commands[1000];
int commandsIndex = 0;

EnvVar var[1000];
int varIndex = 0;

// color, initial as white
char *color = "initial";

// change theme based on color
void changeTheme(char *color){
	if(strcmp(color,"red\0") == 0){
		printf("\033[31m");
		color="red";
	}
	else if(strcmp(color,"green\0") == 0){
		printf("\033[32m");
		color="green";
	}
	else if(strcmp(color,"blue\0") == 0){
		printf("\033[34m");
		color="blue";
	}
	else if(strcmp(color,"initial\0") == 0){
		printf("\033[0m");
		color="initial";
	}
	else{
		printf("Unsupported theme: %s", color);
	}
}

// print prompt "cshell"
void printPrompt(char *color){
	changeTheme(color);
	printf("cshell$ ");
	printf("\033[0m");
}

// read and save input into char *l
void readLine(char *l){
	int index = 0;
	int ch;
	ch = getchar();
	while(ch != '\n'){
		l[index] = ch;
		ch = getchar();
		index++;
	}
	l[index] = '\0';
}

// split a line into tokens through separator, return tokens[]
char** split_save_printTokens(char *l, char *separator){
	char *copy1;
	copy1 = strdup(l);
	char *p1;
	int tokenIndex = 0;
	char **tokens = malloc(8 * sizeof(char *));
	p1 = strtok(l, separator);
	while (p1 != NULL)
    {	
		tokens[tokenIndex] = p1;
        p1 = strtok(NULL, separator);
		tokenIndex++;
    }
	free(copy1);
	tokens[tokenIndex] = NULL;

	return tokens;
}

int getNumOfTokens(char *l, char separator){
	int num = 0;
	int index =0;
	while(l[index]!= '\0'){
		if(l[index] == separator){
			num++;
		}
		index++;
	}
	return num;
}

int find_char (char **strings,char value)
{
    int num=0;
    char *string;
    while((string= *strings++)!=NULL)
    {
        while(*string!='\0')
        {
            if(*string++==value)
            {
             num++;
            }
        }
    }
    return num;
}

// delete '$' in the $ command
void delete_char(char str[],char target)
{
	int i,j;
	for(i=j=0;str[i]!='\0';i++){
		if(str[i]!=target){
			str[j++]=str[i];
		}
	}
	str[j]='\0';
}

// check for if there are ' ' inside $ commmand
int isin(char key,char par[]){
    int len =strlen(par);
    int i=0;
    for(i=0;i<len;i++){
        if(par[i]==key)
            return 1;
    }
    return 0;
}

// for none build in commands, split a line into {" ", " ", " ",    } style
char **get_input(char *input) {
	char **command = malloc(8 * sizeof(char *));
	char *separator = " ";
	char *parsed;
	int index = 0;
	parsed = strtok(input, separator);
	while (parsed != NULL) {
		command[index] = parsed;
		index++;
		parsed = strtok(NULL, separator);

	}
	command[index] = NULL;
	return command;
}
// for string copy
void Mystrcat1(char *des,char *src)
{
	int i = 0;
	while(des[i]!='\0') 
		i++;
	strcpy(&des[i],src);
}

char* executeCommand(char *color, char *line){
		// check the theme of current command
		changeTheme(color);
		// for the exit command
		if(strcmp(line,"exit\0") == 0){
			printf("Bye!\n");
			exit(0);
		}
		// for build-in commands
		// for $<VAR>=<value> command
		if(line[0] == '$'){
			// for incorrect input
			if(isin(' ', line)){
				printf("Variable value expected\n");
			}
			else{
				// save into log command structure
				strcpy(commands[commandsIndex].name,line);
				time_t rawtime = time(NULL);
				time(&rawtime);
				commands[commandsIndex].time = gmtime (&rawtime);
				strcpy(commands[commandsIndex].timeStirng,asctime(commands[commandsIndex].time));
				commands[commandsIndex].value = 0;
				commandsIndex++;

				// delete "$" and get {name, value}
				delete_char(line, '$');
				char **tokens = split_save_printTokens(line, "=");

				// if name same, update value
				int check = 1;
				for(int i = 0; i < varIndex; i++){
					if(strcmp(var[i].name,tokens[0]) == 0){
						strcpy(var[i].value, tokens[1]);
						check = 0;
					}
				}
				// if not same, add new envar struct
				if(check == 1){
					strcpy(var[varIndex].name, tokens[0]);
					strcpy(var[varIndex].value, tokens[1]);
					varIndex++;
				}
			}	
		}
		else{
			// get <command> <arg1> <arg2> ... 
			char **tokens = split_save_printTokens(line, " ");

			if(tokens[0]==NULL){
				printf("please enter a command\n");
			}
			// for the print command
			else if(strcmp(tokens[0],"print\0") == 0){
				if(tokens[1] == NULL){
					// printf("nothing to print\n");
					// printf(" ");
				}
				else if(find_char(tokens, '$') > 0){
					// printf("got you!\n");
					int tokenIndex = 1;
					char notFound[100];
					char copy[100];
					char check = 0;
					while(tokens[tokenIndex]!= NULL){
						if(tokens[tokenIndex][0]== '$'){
							strcpy(copy, tokens[tokenIndex]);
							delete_char(copy, '$');
							int c = 0;
							for(int i = 0; i < varIndex; i++){
								if(strcmp(var[i].name,copy) == 0){
									c =1;
								}
							}
							if(c==0){
								strcpy(notFound, copy);
								check = 1;
							}
						}
						tokenIndex++;
					}
					if(check ==1){
						printf("Error: No Environment Variable $%s found.\n", notFound);
					}
					else{
						int tokenIndex1 = 1;
						char result[100] = "";
						while(tokens[tokenIndex1]!= NULL){
						// printf("%s",tokens[tokenIndex1]);
							if(tokens[tokenIndex1][0]== '$'){
								delete_char(tokens[tokenIndex1], '$');
								for(int i = 0; i < varIndex; i++){
									if(strcmp(var[i].name,tokens[tokenIndex1]) == 0){
										Mystrcat1(result, var[i].value);
										Mystrcat1(result, " ");
									}
								}
							}
							else{
								Mystrcat1(result, tokens[tokenIndex1]);
								Mystrcat1(result, " ");
							}
							tokenIndex1++;
						}
						int resultIndex = 0;
						while(result[resultIndex]!='\0'){
							resultIndex++;
						}
						result[resultIndex-1] = '\0';
						while(result[resultIndex]!= 0){
							result[resultIndex] = '\0';
							resultIndex++;
						}
						printf("%s ", result);
						printf("\n");
					}
				}
				// normal print
				else{
					int tokenIndex = 2;
					char result[100] = "";
					Mystrcat1(result, tokens[1]);
					while(tokens[tokenIndex]!= NULL){	
						Mystrcat1(result, " ");
						Mystrcat1(result, tokens[tokenIndex]);
						tokenIndex++;
					}
					printf("%s ", result);
					printf("\n");
				}

				// save the current command into log command struct
				strcpy(commands[commandsIndex].name,"print");
				time_t rawtime = time(NULL);
				time(&rawtime);
				commands[commandsIndex].time = gmtime (&rawtime);
				strcpy(commands[commandsIndex].timeStirng,asctime(commands[commandsIndex].time));
				commands[commandsIndex].value = 0;
				commandsIndex++;
			}
		
			// for the theme command
			else if(strcmp(tokens[0],"theme\0") == 0){
				
				int tokenIndex = 1;
				if(strcmp(tokens[tokenIndex],"red\0") == 0){
					printf("\033[31m");
					color="red";
				}
				else if(strcmp(tokens[tokenIndex],"green\0") == 0){
					printf("\033[32m");
					color="green";
				}
				else if(strcmp(tokens[tokenIndex],"blue\0") == 0){
					printf("\033[34m");
					color="blue";
				}
				else if(strcmp(tokens[tokenIndex],"initial\0") == 0){
					printf("\033[0m");
					color="initial";
				}
				else{
					printf("Unsupported theme: %s\n", tokens[tokenIndex]);
				}

				// save the current command into log command struct
				strcpy(commands[commandsIndex].name,"theme");
				time_t rawtime = time(NULL);
				time(&rawtime);
				commands[commandsIndex].time = gmtime (&rawtime);
				strcpy(commands[commandsIndex].timeStirng,asctime((commands[commandsIndex].time)));
				commands[commandsIndex].value = 0;
				commandsIndex++;
			}

			// for the log command
			else if(strcmp(tokens[0],"log\0") == 0){
				// print command log from command log structure
				for(int i = 0; i < commandsIndex; i++){
					printf("%s", commands[i].timeStirng);
					printf(" %s", commands[i].name);
					printf(" %d\n", commands[i].value);
				}

				strcpy(commands[commandsIndex].name,"log");
				time_t rawtime = time(NULL);
				time(&rawtime);
				commands[commandsIndex].time = gmtime (&rawtime);
				strcpy(commands[commandsIndex].timeStirng,asctime((commands[commandsIndex].time)));
				commands[commandsIndex].value = 0;
				commandsIndex++;
			}
			
			//handle non build in command
			else{
				int fds[2];
  				pid_t pid;
				
				if(pipe(fds) == -1){
    				perror("Eroor: ");
    				exit(1);
  				}
				pid = fork();

				if (pid < 0) {
					perror("fork Failed");
					exit(1);
				}
				// parent process
				if(pid > 0){
					wait(NULL);
					char childProcessCommandName[20];
					close(fds[1]);//close write end
					read(fds[0],childProcessCommandName, sizeof(childProcessCommandName));

					strcpy(commands[commandsIndex].name,childProcessCommandName);
					time_t rawtime = time(NULL);
					time(&rawtime);
					commands[commandsIndex].time = gmtime (&rawtime);
					strcpy(commands[commandsIndex].timeStirng,asctime(commands[commandsIndex].time));
					commands[commandsIndex].value = 0;
					commandsIndex++;
				}
				else{
					char* childColor = color;
					printPrompt(childColor);
					char **nonBuiltInCommand;
					nonBuiltInCommand = get_input(line);
					close(fds[0]);//close read end
    				write(fds[1], line, sizeof(line));
					execvp(nonBuiltInCommand[0], nonBuiltInCommand);

					printf("Missing keyword of command, or permission problem\n");
					exit(1);
				}
			}		
		}
	return color;
}

void interactive_mode(){
	char *line  = NULL;
	line = (char *)malloc(256);

	while(1){
		// Print out a prompt
		printPrompt(color);
		// Read line
		readLine(line);
		char *afterColor = executeCommand(color,line);
		color = afterColor;		
	}
}

void scriptMode(char *fileName){
	FILE *pFile;
	// pFile = fopen("testScript.txt", "r");
	pFile = fopen(fileName, "r");
	char szTest[1000] = {0};
	if(NULL == pFile)     
	{         
		printf("failed to open file\n");         
		exit(1);     
	}
	while(!feof(pFile))  
	{          
		memset(szTest,0, sizeof(szTest));                 
		fgets(szTest,sizeof(szTest) - 1, pFile);    
		delete_char(szTest ,'\n');
		if(szTest[strlen(szTest)-1] == '\r'){
			szTest[strlen(szTest)-1] = '\0';  
		}
		if(szTest[0] == 0){
			printf("Bye!\n");
			exit(0);
		}
		char *afterColor = executeCommand(color,szTest);
		color = afterColor;
	}
	fclose(pFile);  
}
int main(int argc, char *argv[]){	
	if(argc==1){
		interactive_mode();
	}
	else{
		scriptMode(argv[1]);
	}

	return 0;
}