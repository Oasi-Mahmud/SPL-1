#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "token.h"
#include "summary.h"
#include "style_rules.h"
#include "bug_rules.h"

static int is_type_keyword(const char *s) {
    return strcmp(s,"int")==0 || strcmp(s,"double")==0 ||
           strcmp(s,"float")==0 || strcmp(s,"char")==0 ||
           strcmp(s,"boolean")==0 || strcmp(s,"long")==0 ||
           strcmp(s,"short")==0;
}

void analyze(Token *t, int n) {

int classCount=0,methodCount =0;
    int forCount =0, hileCount=0, doCount = 0;
    int globalVar =0,localVar=0;
    int LOC=0;
    int braceDepth=0;
    int insideMethod=0;

    char methodNames[100][64];
    char classNames[100][64];
    char classParent[100][64];   
    char classInterface[100][64];
    int isAbstractClass[100]; 
    int isInterfaceClass[100]; 
    int methodLines[100];
    int classLines[100];

    printf("\nDetected Interfaces:\n");

    for(int i =0;i<n-1;i++){
        if(t[i].type ==TOK_KEYWORD &&
             strcmp(t[i].lexeme, "interface") ==0 &&
            t[i+1].type==TOK_IDENTIFIER) {

            printf("  Interface: %s (line %d)\n",
                t[i+1].lexeme, t[i].line);
        }
    }

    printf("\nDetected Classes:\n");

      for (int i=0;i< n-1;i++) {
        if (t[i].type ==TOK_KEYWORD &&
            strcmp(t[i].lexeme,"class") ==0 &&
            t[i+1].type ==TOK_IDENTIFIER) {
                int abstractFlag=0;
                    if (i>0 && t[i-1].type==TOK_KEYWORD &&
                         strcmp(t[i-1].lexeme, "abstract") ==0){
                        abstractFlag=1;
                    }
                    char parentName[64]="None";
                    char interfaceName[64]="None";
    
                    for(int j=i+2;j< n-1;j++){
                        if (t[j].type ==TOK_KEYWORD &&
                            strcmp(t[j].lexeme, "extends")==0 &&
                            t[j+1].type==TOK_IDENTIFIER) {
    
                            strcpy(parentName,t[j+1].lexeme);
                            break;
                        }
                        if (t[j].type==TOK_KEYWORD &&
                            strcmp(t[j].lexeme, "implements") == 0 &&
                            t[j+1].type== TOK_IDENTIFIER) {
    
                            strcpy(interfaceName, t[j+1].lexeme);
                        }
    
                        if (t[j].type ==TOK_SYMBOL && strcmp(t[j].lexeme, "{") == 0) {
                            break;
                    }
                }


            printf("  Class: %s (line %d)\n",
                   t[i+1].lexeme,t[i].line);
                    if(abstractFlag){
                        printf("    -> This is an ABSTRACT class\n");
                    }
                    if (strcmp(parentName, "None") !=0) {
                        printf("    -> Inherits from: %s\n", parentName);
                    }
                    if (strcmp(interfaceName, "None") !=0) {
                        printf("    -> Implements: %s\n", interfaceName);
                    }
             if(classCount<100){
                strcpy(classNames[classCount],t[i+1].lexeme);
                classLines[classCount] = t[i].line;
                isAbstractClass[classCount] = abstractFlag;
                strcpy(classParent[classCount], parentName);
                strcpy(classInterface[classCount], interfaceName);
            }
            classCount++;
        }
    }

    printf("\nDetected Methods:\n");

    for (int i=0; i<n-2;i++) {
        if ((t[i].type==TOK_KEYWORD && is_type_keyword(t[i].lexeme)) &&
            t[i+1].type ==TOK_IDENTIFIER &&
            strcmp(t[i+2].lexeme, "(")==0){

            printf("  Method: %s (line %d)\n",
                   t[i+1].lexeme,t[i].line);
             if(methodCount<100){
                strcpy(methodNames[methodCount], t[i+1].lexeme);
                methodLines[methodCount]=t[i].line;
            }
            methodCount++;
        }

        if (t[i].type==TOK_KEYWORD &&
            strcmp(t[i].lexeme,"void")==0 &&
            t[i+1].type ==TOK_IDENTIFIER &&
            strcmp(t[i+2].lexeme,"(")==0){

            printf("  Method: %s (line %d)\n",
                   t[i+1].lexeme, t[i].line);
            if(methodCount<100) {
                strcpy(methodNames[methodCount], t[i+1].lexeme);
                methodLines[methodCount] = t[i].line;
            }
            methodCount++;
        }
    }

    printf("\nDetected Loops:\n");

    for(int i=0;i< n;i++){
        if(t[i].type ==TOK_KEYWORD) {
            if(strcmp(t[i].lexeme,"for")==0){
                printf("for-loop (line %d)\n", t[i].line);
                forCount++;
            }
            if(strcmp(t[i].lexeme,"while")==0){
                printf("  while-loop (line %d)\n", t[i].line);
                whileCount++;
            }
            if(strcmp(t[i].lexeme,"do")==0){
                printf("  do-while-loop (line %d)\n", t[i].line);
                doCount++;
            }
        }
    }

     printf("\nDetected Variables:\n");

    int variableCount=0;

    for(int i=0;i<n-1;i++){

    if(t[i].type ==TOK_KEYWORD && is_type_keyword(t[i].lexeme) &&
        t[i+1].type== TOK_IDENTIFIER){

        printf("  Variable: %s (line %d)\n",
               t[i+1].lexeme, t[i].line);

        variableCount++;
    }
}
    int lastLine=-1;                 
    for (int i=0;i<n;i++) {
        if (t[i].line!=lastLine &&
            t[i].type!=TOK_EOF) {
            LOC++;
            lastLine=t[i].line;
        }
    }

    printf("\nSummary  :\n");
    printf("  Total Classes        : %d \n", classCount);
    printf(" Total Methods        : %d \n", methodCount);
    printf("  for-loops            : %d\n", forCount);
    printf(" while-loops          : %d\n", whileCount);
    printf("  do-while-loops      : %d\n", doCount);
    printf("  Global Variables    : %d\n", globalVar);
    printf("  Local Variables    : %d\n", localVar);
    printf("  Lines of Code (LOC)   : %d\n", LOC);

    run_style_checks(t,n);
    run_bug_checks(t,n);

    char choice;
    printf("\nDo you want to see summary? (y/n): ");
    scanf(" %c", &choice);

    if (choice == 'y' || choice == 'Y') {

        printf("\n1. Method Summary\n");
        printf("2. Class Summary\n");
        printf("Select option: ");

        int opt;
        scanf("%d",&opt);

        if (opt == 1 && methodCount>0) {

            printf("\nAvailable Methods:\n");

            int printableMethods = (methodCount < 100) ? methodCount : 100;

            for (int i = 0; i<printableMethods;i++)
                printf("  %d. %s\n",i+1,methodNames[i]);

            int sel;
            printf("Enter method number: ");
            scanf("%d", &sel);

            if(sel >=1&& sel<=printableMethods){
                char buffer[256];
                method_name_summary(methodNames[sel-1], buffer, sizeof(buffer));

                printf("\nMethod Summary:\n");
                printf("  Name    : %s\n", methodNames[sel-1]);
                printf("  Line    : %d\n", methodLines[sel-1]);
                printf("  Purpose : %s\n", buffer);
            }
}
 else if(opt==2&&classCount>0){

            printf("\nAvailable Classes:\n");

            int printableClasses=(classCount < 100) ? classCount : 100;

            for (int i=0; i<printableClasses;i++){
                printf("  %d. %s\n", i+1, classNames[i]);
            }
            int sel;
            printf("Enter class number: ");
            scanf("%d", &sel);

            if(sel >=1&&sel<=printableClasses){
                char buffer[256];
                class_name_summary(classNames[sel-1], buffer,sizeof(buffer));

                printf("\nClass Summary:\n");
                printf("  Name    : %s\n",classNames[sel-1]);
                printf("  Line    : %d\n",classLines[sel-1]);
                printf("  Purpose : %s\n",buffer);
            }
        }
    }
}


int main(){
 
    char dirPath[260];
    printf("Enter directory path: ");
    fgets(dirPath, sizeof(dirPath),stdin);

    size_t L =strlen(dirPath);
    if (L>0&&(dirPath[L-1]=='\n' || dirPath[L-1] == '\r')) dirPath[L-1] = '\0';
    L = strlen(dirPath);
    if (L >0 && dirPath[L-1] ='\r') dirPath[L-1] = '\0';

    char pattern[300];
    snprintf(pattern, sizeof(pattern), "%s\\*.java", dirPath);

     WIN32_FIND_DATAA ffd;
    HANDLE hFind=FindFirstFileA(pattern, &ffd);

    if(hFind==INVALID_HANDLE_VALUE){
        printf("No .java files found (or directory invalid).\n");
        return 1;
    }

    char files[200][260];
    int fileCount=0;

    printf("\nAvailable .java files:\n");

    do{
        if(!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if(fileCount<200){
                strcpy(files[fileCount], ffd.cFileName);
                printf("  %d. %s\n", fileCount+1,files[fileCount]);
                fileCount++;
            }
        }
    }
    while(FindNextFileA(hFind, &ffd) !=0);

    FindClose(hFind);
    
     if(fileCount==0){
        printf("No .java files found.\n");
        return 1;
    }
    
    int choice=0;
    printf("\nSelect file number: ");
    scanf("%d", &choice);

    if(choice<1 || choice>fileCount){
        printf("Invalid selection.\n");
        return 1;
    }

    char fullPath[520];
    snprintf(fullPath, sizeof(fullPath), "%s\\%s", dirPath, files[choice - 1]);

    int count=0;
    Token *tokens=lex_file(fullPath, &count);
    
    if(!tokens){
        fprintf(stderr,"Tokenization failed\n");
        return 1;
    }

    analyze(tokens,count);
    free_tokens(tokens, count);
    return 0;
}

