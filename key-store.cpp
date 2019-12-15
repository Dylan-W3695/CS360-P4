#include <iostream>
#include <pthread.h>
#include <string>
#include <stdio.h>
#include <queue>
#include <list>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <bits/stdc++.h>

using namespace std;

queue<string> handleArgs(int argc, char** argv);
void storeKeyAndValue(string store_key, string value);
void storeKeyAndFileContents(string store_key, string file);
string retrieveKeyValue(string retrieve_key);
string retrieveKeyAndCopyToFile(string retreive_key, string file);
int openWriteFile(string file);
int readKeyFile(string keyFileDescriptor, string key );
string getValue(string valueFileDescriptor, int line_number);

string keyFile = ".keys";
string valueFile = ".values";

int main(int argc, char** argv)
{
  string store_key = "";
  string retrieve_key= "";
  string value="";
  string file="";
  queue<string> argQueue =  handleArgs(argc,argv);

  //Max for queue is 4 values, since we also store a descriptor before the actual value of the arg
  // E.g. argQueue = ["s", "store_value", 'f', "file_name"]
  
  if(argQueue.empty() || argQueue.size() > 4) //Queue should have at most 4 args, and should not be empty
    {
      cout << endl <<"key-store <--store key> | <--retrieve key> <value | --file file_path> [--help]" << endl;
      exit(EXIT_SUCCESS);
    }
  
  if(!argQueue.empty())
    {
      string first_desc = argQueue.front();
      argQueue.pop();

      // If our first descriptor is a store flag...
      if(first_desc == "s")
	{
	  // The next arg will be the key for the store
	  store_key = argQueue.front();
	  argQueue.pop();

	}
      else if(first_desc == "r")
	{
	  {
	    // The next arg will be the key for the store
	    retrieve_key = argQueue.front();
	    argQueue.pop();
	  }
	  
	}

       // Get the descriptor flag for the second arg
	  string second_desc = argQueue.front();
	  argQueue.pop();
      
      // If it's a value, run storeKeyAndValue
      if(second_desc == "v")
	{
	      value = argQueue.front();
	      argQueue.pop();
	    }
      
      // Or if its a file, run storeKeyAndFileContents
      else if(second_desc == "f")
	    {
	      file = argQueue.front();
	      argQueue.pop();
	    }
    }
  
  if(store_key != "")
    {
      if(value != "")
	{
	  storeKeyAndValue(store_key, value);
	}

      else if(file != "")
	{
	  storeKeyAndFileContents(store_key, file);
	}
    }
  else if(retrieve_key != "")
    {
      // Retrieve
        if(file == "")
	{
	  retrieveKeyValue(retrieve_key); 
	}

	else if(file != "")
	{
	  retrieveKeyAndCopyToFile(retrieve_key, file);
	}
    }
  else
    {
      cerr << "ERROR" << endl;
      exit(EXIT_FAILURE);
    }
  
  exit(EXIT_SUCCESS);
}


queue<string> handleArgs(int argc, char** argv){

  //store, retrieve, and file all require arguments, so are followed by a colon in optString. Help does not require one.
  const char* const optString = "s:r:f:h";
  queue<string> argQueue;
  const option fullOpts[] =
    {
      {"store", required_argument, NULL, 's'},
      {"retrieve", required_argument, NULL, 'r'},
      {"file", required_argument, NULL, 'f'},
      {"help", no_argument, NULL, 'h'}
    };

  while(true)
    {
      const int opt = getopt_long(argc, argv, optString, fullOpts, NULL);
      
      if(opt == -1)
	{
	  return argQueue;
	}

      switch(opt)
	{
	case 'h':
	  {
	    cout << endl <<"key-store <--store key> | <--retrieve key> <value | --file file_path> [--help]" << endl;
	    exit(EXIT_SUCCESS);
	  }
	case 's':
	  {
	    string str = "--file";
	    string arg3(argv[3]); 
	    cout <<"Got command to STORE: " <<  optarg << endl;
	    argQueue.push("s");
	    string store_key = optarg;
	    argQueue.push(store_key);

	    if(arg3.compare(str) != 0 )
	      {
		argQueue.push("v");
		string store_value = argv[3];
		argQueue.push(store_value);
	      }
	    
	    break;
	    
	  }

	case 'r':
	  {
	    cout << "Got command to RETRIEVE: " << optarg << endl;
	    argQueue.push("r");
	    string retrieve_value = optarg;
	    argQueue.push(retrieve_value);
	    break;
	  }

	case 'f':
	  {
	    cout << "Got FILE arg: " << optarg << endl;
	    argQueue.push("f");
	    string file_value = optarg;
	    argQueue.push(file_value);
	    break;
	  }
	  
	default:
	  {
	    // If they didn't add any args, print help signature and exit
	    cout << endl <<"key-store <--store key> | <--retrieve key> <value | --file file_path> [--help]" << endl;
	    exit(EXIT_SUCCESS);
	  }
	}
    }

  return argQueue;
}

int openWriteFile(string file){

    int wFileDescriptor = open(file.c_str(), O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if (wFileDescriptor < 0){
	cerr << "failed to open " << file;
	exit(EXIT_FAILURE);
    }
    return wFileDescriptor;
}

int openReadFile(string file){
  int rFileDescriptor = open(file.c_str(), O_RDONLY);
    if(rFileDescriptor == -1){
	cerr << "failed to open input file: "<< file;
	exit(EXIT_FAILURE);
    }
    return rFileDescriptor;
}

  
void storeKeyAndValue(string store_key,string value)
{
  cout << "Reached store Key and Value" << endl;
  int keyFileDescriptor = openWriteFile(keyFile);
  int valueFileDescriptor = openWriteFile(valueFile);

  int keyLength = store_key.length();
  char key_array[keyLength + 2];
  strcpy(key_array, store_key.c_str());

  for(int i = 0; i < keyLength; i ++){
      int writeStat = write(keyFileDescriptor, &key_array[i], sizeof(char));
      if(writeStat == -1){
	  cerr << ("Failed to Write Key");
	  exit(EXIT_FAILURE);
      }
  }


  int valueLength = value.length();
  char value_array[valueLength];
  strcpy(value_array, value.c_str());

  for(int i = 0; i < valueLength; i ++){
      int writeStat = write(valueFileDescriptor, &value_array[i], sizeof(char));
      if(writeStat == -1){
          cerr << ("Failed to Write value");
          exit(EXIT_FAILURE);
      }
  }

   write(keyFileDescriptor, "\n", 1);
   write(valueFileDescriptor, "\n", 1);

  
  close(keyFileDescriptor);
  close(valueFileDescriptor);

}

void storeKeyAndFileContents(string store_key,string file)
{
  cout << "Reached store Key and File" <<endl; 
  int wKeyFileDescriptor = openWriteFile(keyFile);
  int wValueFileDescriptor = openWriteFile(valueFile);

  if(readKeyFile(keyFile, store_key) == -1){

      int rValueFileDescriptor = openReadFile(file);

      int keyLength = store_key.length();
      char key_array[keyLength + 2];
      strcpy(key_array, store_key.c_str());

      for(int i = 0; i < keyLength; i ++){
	  int writeStat = write(wKeyFileDescriptor, &key_array[i], sizeof(char));
	  if(writeStat == -1){
	      cerr << ("Failed to Write Key");
	      exit(EXIT_FAILURE);
	  }
      }

      int bytesRead;
      char byte;
      while((bytesRead = read(rValueFileDescriptor, &byte, sizeof(char))) > 0){
	  if(write(wValueFileDescriptor, &byte, sizeof(char))  == -1){
	      cerr << "Failed to write file";
	      exit(EXIT_FAILURE);
	  }
      }
   write(wKeyFileDescriptor, "\n", 1);
   write(wValueFileDescriptor, "\n", 1);

  }else{
      cout << "key already exists" << endl;
  }
}

string retrieveKeyValue(string retrieve_key)
{
  
  //int keyFileDescriptor = openWriteFile(keyFile);
  // int valueFileDescriptor = openWriteFile(valueFile);

  //First, find the .key value in .keys, and count how many newlines down the file it is  
  int line_number;
  line_number = readKeyFile(keyFile, retrieve_key);

  //Then, get the value that is line_number newlines down inside the .values file
  string value = getValue(valueFile, line_number);

  
  cout << "reached retrieve Key and Value," << endl;
  cout << endl << "I've retrieved the following value associated with " << retrieve_key << " : "  <<  value << endl;
  return value;
}

int readKeyFile(string keyFileDescriptor, string key )
{
  int line_number = -1;
  int final_line = -1;
  
  // Open, read, and copy the picture
  // Open bunny.jpg then copy it into bunny_copy.jpg
  char textBuffer;

  // Open the file
  int textID = open(keyFileDescriptor.c_str(), O_RDONLY|O_LARGEFILE);
  if( textID < 0)
    {
      perror("Failed to open .keys file.");
      exit(EXIT_FAILURE);
    }

 
  // Read and copy the file
  ssize_t bytesRead;
  string line_so_far;
  bool still_searching = true;
  
  while( bytesRead = read(textID, &textBuffer, sizeof(char)) > 0 && still_searching)
    {
      /* while(textbuffer != "\n")
	 {
	    push char to string
	    
	 }
	 compare string to key
	 increment line_number
	 
       */

      while(textBuffer != '\n')
	{
	  // While we haven't reached a newline, append current char to line_so_far 
	  line_so_far += textBuffer;

	  //If we reach the end of the file while inside this loop, break
	  if(read(textID, &textBuffer, sizeof(char)) == 0)    break;
	}
      
      if(!line_so_far.compare(key) == 0)
	{
	  // We've reached a new line and the line we have is NOT the key,
	  // so increment line_number and reset line_so_far
	  line_number++;
	  line_so_far = "";
	}
      else
	{
	  still_searching = false;
	  final_line = line_number;
	}
    }  


  if(bytesRead < 0)
    {
      perror("Failed to read .keys file.");
      exit(EXIT_FAILURE);
    }

  close(textID);
  return final_line;
}

string getValue(string valueFileDescriptor, int line_number)
{ 
  int wValueFileDescriptor = openWriteFile(valueFile);
  char textBuffer;
  
  int textID = open(valueFileDescriptor.c_str(), O_RDONLY|O_LARGEFILE);
  if( textID < 0)
    {
      perror("Failed to open .values file.");
      exit(EXIT_FAILURE);
    }

 
  // Read and copy the file
  ssize_t bytesRead;
  string valueString = "";
  int current_line_count = -1;

  // While we haven't reached the end of the file...
  while( bytesRead = read(textID, &textBuffer, sizeof(char)) > 0)
    {
      // If we encounter a newline, increment current_line_count
      if(textBuffer == '\n') current_line_count++; 
      
      // If we've reached the appropriate line_number, read the entire line and return it
      if(current_line_count == line_number)
	{
	  // While we haven't reached a new line...
	  while(textBuffer != '\n')
	    {
	      // Add the current char in textBuffer to our final string
	      valueString += textBuffer;
	      if(read(textID, &textBuffer, sizeof(char)) == 0)    break;
	    }
	}
    } 

  if(bytesRead < 0)
    {
      perror("Failed to read .keys file.");
      exit(EXIT_FAILURE);
    }

  close(textID);
  return valueString;
  
}

string retrieveKeyAndCopyToFile(string retrieve_key,string  file)
{
  cout << "reached retrieve key and file" << endl;
  string value = retrieveKeyValue(retrieve_key);

    int wFileDescriptor = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (wFileDescriptor < 0){
        cerr << "failed to open " << file;
        exit(EXIT_FAILURE);
    }
    const char* valuestr = value.c_str();
    for(int i = 0; i < value.length(); i++){
	if(write(wFileDescriptor, &valuestr[i], sizeof(char)) == -1){
	    cerr << "Failed to write to file: " << file;
	    exit(EXIT_FAILURE);
	}
    }
    return value;
}
