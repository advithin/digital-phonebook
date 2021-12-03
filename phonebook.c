//C program for phonebook; uses tries data structure for suggestions of key and finding key value
//of the key value
//last 10 number is considered the phone number and the first are the extensions
// not using hash because of collisions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<ctype.h>
#include <stdbool.h>
#define ALPHABET_SIZE (26)// Converts key current character into index
#define CHAR_TO_INDEX(c) ((int)c - (int)'a')// use only 'a' through 'z' and lower case
#define FREE(p)\
    free(p);   \
    p=NULL;

struct TrieNode// trie node
{
    int value;
    struct TrieNode *children[ALPHABET_SIZE];// children is an array of pointers
    bool isEndOfWord;// isEndOfWord is true if the node represents end of a word
    unsigned long int *phone_num;// making it a pointer if there are multiple numbers// using unsigned long as the max possible number is 9999999999 + ext which is more than 32 bits accomodating by an integer
    char* email;
};// Returns new trie node (initialized to NULLs)
struct TrieNode *getNode(void)
{
    struct TrieNode *pNode = NULL;
    pNode = (struct TrieNode *)malloc(sizeof(struct TrieNode));
    if (pNode)
    {
        int i;
        pNode->value=0;
        pNode->isEndOfWord = false;
        for (i = 0; i < ALPHABET_SIZE; i++)
            pNode->children[i] = NULL;
        pNode->phone_num = NULL;
                pNode->email = NULL;
    }
    return pNode;
}

/***********************************************************
* Function: Inserts data into trie
* Arguments: root of trie node, name of person, list of phone numbers in pointer form
************************************************************/
void insert(struct TrieNode *root,char *key, unsigned long int phone_number,char *email_addr)// Insert keys into the Trie
{
    int level,j=0;
    int length = strlen(key);
    int index;
    struct TrieNode *temp = root;
    for (level = 0; level < length; level++)
    {
        index = CHAR_TO_INDEX(key[level]);
        if (!temp->children[index])
            temp->children[index] = getNode();
        temp = temp->children[index];
    }
    temp->value=10;
    temp->isEndOfWord = true; // mark last node as leaf
    if (!temp->phone_num)// phone numbers are assigned to leaf
         temp->phone_num = (unsigned long int*)malloc (sizeof(unsigned long int));
    	 if (!temp->email)
			temp->email = (char *)malloc (sizeof(char)*strlen(email_addr));
    *temp->phone_num =  phone_number;
	 for (j=0;j<strlen(email_addr);j++)
	 	temp->email[j] = email_addr[j];
	 temp->email[j]='\0';

}
// FIXME delete function
// FIXME modify number function
// FIXME display option of dumping data into file
// FIXME search when user enters query, should show suggestions based on prefix.
// use for debug only

void delete (struct TrieNode *root, char *name ) {
	struct TrieNode * temp, * last_valid;
	temp = root;
	int i, index, found, length, last_index;
	length = strlen(name);
	last_index = -1;
	last_valid = NULL;
	for (i =0; i< length;i++) {
		index = CHAR_TO_INDEX(name[i]);
		if (temp->children[index]) {
			found = 1;
			temp = temp->children[index];
			if (i != length -1 && temp->isEndOfWord) {
				last_valid  = temp;
				last_index = i+1;
			}
		} else {
			break;
		}
	}
	struct TrieNode *cur, *head;
	cur = temp;
	int num_child = 0,j;
	//printf("last index %d\n",last_index);
	// First check if contact was found
	if (found == 1 && i == length && temp->isEndOfWord) {
		// Contact found
		// Now check if contact has any children
		for (j =0 ; j< 26;j++) {
			if (temp->children[j]) {
				num_child = 1;
				break;
			}
		}
		// check num_child
		if ( num_child == 1) {
			// cannot break succession, so simply remove validity
			cur->isEndOfWord = false;
			// FIXME tmp_num = cur->phone_num;
			// cur->phone_num = NULL;
			// delete(tmp_num);
		} else {
			// Has no children, free to remove nodes from trie
			if (!last_valid) {
				head = root;
				last_index= 0;
			} else {
				head = last_valid;
			}
			// before removing, check for any common predecessors
			temp = head;
			index = CHAR_TO_INDEX(name[last_index]);
			int new_index = index;
			temp = temp->children[index];
			//printf ("inside\n");
			while(temp!=cur) {
				num_child =0;
				last_index++;
				index = CHAR_TO_INDEX(name[last_index]);
				for (i=0;i<26;i++) {
					if (temp->children[i] && (index!=i)) {
						num_child = 1;
						break;
					}
				}
				if (num_child == 1) {
					head = temp;
					new_index = index;
				}
				temp = temp->children[index];
			}
			// index points to current, so point the parent to null
			// now assign the child to null
			head->children[new_index] = NULL;
		}
		printf ("Contact found and deleted\n");
	} else {
		printf("contact not found, skipping.\n");
	}

}

void display (struct TrieNode *root) {
	struct TrieNode *temp;
	temp = root;
	int i;
	char *alpha;
	alpha = (char *) malloc (sizeof(char)*100);
	printf("\n*****************************************************************************\n");
	printf ("\t \t \t ---Contacts List---\n");
	for (i=0;i<26;i++) {
		if (root->children[i]) {
			alpha[0] = 'a' + i;
			temp = root->children[i];
			recursive_search(temp,1,alpha);
		}
	}
printf("\n*****************************************************************************\n");
}
/***********************************************************
* Function: Prints trie with each character with depth information. Use for printing during debug only
* Arguments: root of trie node, depth of root = 0
************************************************************/
void printTrie (struct TrieNode *root, int level ) {
   if (!root) return;
   int i,k=0;
   if (root->isEndOfWord)
   {
       printf("%d \n",*root->phone_num);
       	for (k =0;root->email[k]!='\0';k++)
			printf("%c",root->email[k]);
		printf("\n");
   }
   for (i = 0; i < 26; i++) {
      if (root->children[i]) {
         printf ("%c %d \n",('a' + i),level);
         printTrie(root->children[i],level+1); //recursive
      }
   }
}

void modify (struct TrieNode *root, char *search_name, unsigned long int new_num,char *newmail) {
	struct TrieNode *temp;
	char temp_var[30];
	int i=0;
	temp = root;
	int level, index, found =0,k;
	int length = strlen(search_name);
	for (level = 0; level < length;level++) {
		index = CHAR_TO_INDEX(search_name[level]);
		if (temp->children[index]) {
			found = 1;
			temp = temp->children[index];
		} else {
			break;
		}
	}
	if ((found == 1) && (temp->isEndOfWord)  && (level == length)) {
		printf("Found contact!,modifying phone number and email-id.....\n");
		*temp->phone_num = new_num;
		strcpy(temp->email,newmail);
}
}

void print_string (char * str, int len) {
	int i;
	for (i=0;i<len;i++) {
		printf("%c",str[i]);
	}
}
void recursive_search (struct TrieNode *root, int level, char *prefix) {
	if (!root) { return ; }
	int i,k=0;
	if (root->isEndOfWord) {
		printf("\nName: ");
		print_string(prefix,level);
		printf ("\t\tNumber: %lu ",*root->phone_num);
       	printf("\t\tEmail:");
       	for (k =0;root->email[k]!='\0';k++)
			printf("%c",root->email[k]);
	}
	//printf("\n");
	for (i =0; i< 26;i++) {
		if (root->children[i]) {
			prefix[level] = 'a' + i;
			recursive_search(root->children[i], level+1, prefix);
		}
	}
}

void search ( struct TrieNode *root, char * search_name) {
	struct TrieNode *temp;
	temp = root;
	int level, index, found = 0, found_partial = 0,k;
	char  c;
	int length = strlen(search_name);
	char *prefix = (char *) malloc (sizeof(char)*100);
	for (level = 0;level < length;level++) {
		index = CHAR_TO_INDEX(search_name[level]);
		prefix[level] = search_name[level];
		if (temp->children[index]) {
			temp = temp->children[index];
			found_partial = 1;
		} else
			break;
	}
	// Case 1 no match
	// Case 2 all characters match
	// Case 3 all characters match but the whole word is not a valid contact
	if ( found_partial == 0) {
		printf("Name does not exist in the contact list\n");
		return;
	}
	if (level == length && found_partial == 1) {
		if (temp->isEndOfWord) {
			printf("Name found\n");
			printf("Phone number is : %d\n",*temp->phone_num);
			  	for (k =0;root->email[k]!='\0';k++)
			printf("%c",root->email[k]);
			return;
		} else {
			printf("Contact not found. Suggestions : \n");
			recursive_search(temp,level,prefix);
		}
	} else {
		printf("whole name not found\n");
	}

}

/*int countDigits(unsigned long int num)
               {
                   int count=0;
                   while(num)
                   {
                       num=num/10;
                       count++;

                   }
                   return count;
               }
int countDigits(unsigned long int);*/
int countDigits(unsigned long int num)
               {
                   int count=0;
                   while(num)
                   {
                       num=num/10;
                       count++;

                   }
                   return count;
               }
int countDigits(unsigned long int);
int main()
{
    system("COLOR 6C");
    system("COLOR FC");

    struct TrieNode *root = getNode();
    char key[20],prefix[10];int k,i;
    char a[20];
    unsigned long int phone_n;
    int yes;int s;
    char searchname[20];
    unsigned long int newnum;
    char email[100];
    char newmail[100];
   // FIXME check for valid number value before inserting
   do
   {
   printf("What would you like to do?\n");
   printf("1. Insert \n");
   printf("2. Search \n");
   printf("3. Delete \n");
   printf("4. Modify \n");
   printf("5. Display \n");
   int choice;
   printf("Enter your choice: ");
   scanf("%d",&choice);

   switch(choice)
   {
	   case 1: printf("Enter contact name:");
	           scanf("%s",&key);

	         //  printf("How many numbers does a person have?");
	         //  scanf("%d",&k);
	        //   for(i=0;i<k;i++)
	           printf("Enter contact number:");
               scanf("%lu",&phone_n);
                printf("Enter email id:");
               scanf("%s",email);
               s=countDigits(phone_n);
               if(s<10 || s>10){
                    printf("Enter a valid phone number");}
                    else{
                        printf("Valid");
               }
	           insert(root,key,phone_n,email);
				break;
	   case 2: printf("Enter name of contact to be searched:");
               scanf("%s",&prefix);
	           search(root,prefix);
	            break;
       case 3:  printf("Enter contact name to be deleted:");
	            scanf("%s",&key);
                delete(root,key);
                break;
       case 4: printf("Enter contact to be modified:");
               scanf("%s",&searchname);
               printf("Enter new value of number:");
               scanf("%lu",&newnum);
               printf("Enter new email id: ");
               scanf("%s",&newmail);
               modify(root,searchname,newnum,newmail);
               //printTrie(root,1);
	            break;
	   case 5: display(root);
				break;
	   default: printf("Wrong choice, Please try again");
   }printf("\nDo you wish to continue?(Press 1 to continue)");scanf("%d",&yes);
   }while(yes);
    printTrie(root,1);
   // FIXME add input features
   // add scanf function here to accept input from user
   // also add option to read from file and populate
   // FIXME create menu for user

    return 0;
}
