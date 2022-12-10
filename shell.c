// Gabe and Bruce's Operating System Shell
// Following Dr. Margaret Black's instructions and documentation
//  shell.c

void recognizeCmd(char*);
int divideForSectors(int );

void main() {
	char line[80];

	while(1){
		syscall(0, "A:> ");
		syscall(1, line);
		//syscall(0, "Bad command!\r\n");
		recognizeCmd(line);
	}
}

void recognizeCmd(char* name){
	// definitely need to refactor this so each command calls a function
	// rather than refer to a part of this superfunction
	char cBuffer[13312];
    char inputText[80];
    int inputI;
	int nameNum;
	int nameSum;
	int nameMatch;
	int sectorsRead;
    int a;
    int sectorsCounted;

	char cmdName[6];
    char copyName[7];
	char typeCmd[5];
	char execCmd[5];
	char dirCmd[4];
	char delCmd[4];
    char copyCmd[5];
    char makeCmd[7];

	//variables for printing out directory contents
	int fileEntry;
	int entryByte;
	char dir[512];
	char outChars[20];

	typeCmd[0] = 't';
	typeCmd[1] = 'y';
	typeCmd[2] = 'p';
	typeCmd[3] = 'e';
	typeCmd[4] = ' ';

	execCmd[0] = 'e';
	execCmd[1] = 'x';
	execCmd[2] = 'e';
	execCmd[3] = 'c';
	execCmd[4] = ' ';

	dirCmd[0] = 'd';
	dirCmd[1] = 'i';
	dirCmd[2] = 'r';

	delCmd[0] = 'd';
	delCmd[1] = 'e';
	delCmd[2] = 'l';
	delCmd[3] = ' ';

    copyCmd[0] = 'c';
    copyCmd[1] = 'o';
    copyCmd[2] = 'p';
    copyCmd[3] = 'y';
    copyCmd[4] = ' ';

    makeCmd[0] = 'c';
    makeCmd[1] = 'r';
    makeCmd[2] = 'e';
    makeCmd[3] = 'a';
    makeCmd[4] = 't';
    makeCmd[5] = 'e';
    makeCmd[6] = ' ';

    nameMatch = 1;

	for (nameNum = 0; nameNum < 5; nameNum++){
        if(name[nameNum] != typeCmd[nameNum]){
			nameMatch = 0;
			break;
		}
	}

	if (nameMatch < 1){
		nameMatch = 2;
		for (nameNum = 0; nameNum < 5; nameNum++){
			if(name[nameNum] != execCmd[nameNum]){
				nameMatch = 0;
			break;
			}
		}

	}

	if (nameMatch < 1){
        nameMatch = 3;
		for (nameNum = 0; nameNum < 3; nameNum++){
			if(name[nameNum] != dirCmd[nameNum]){
				nameMatch = 0;
			break;
			}
		}
	}

	if (nameMatch < 1){
        nameMatch = 4;
		for (nameNum = 0; nameNum < 4; nameNum++){
			if(name[nameNum] != delCmd[nameNum]){
				nameMatch = 0;
			break;
			}
		}
	}

    if (nameMatch < 1){
        nameMatch = 5;
        for (nameNum = 0; nameNum < 5; nameNum++){
			if(name[nameNum] != copyCmd[nameNum]){
				nameMatch = 0;
			break;
			}
		}
    }

    if (nameMatch < 1){
        nameMatch = 6;
        for (nameNum = 0; nameNum < 7; nameNum++){
            if(name[nameNum] != makeCmd[nameNum]){
				nameMatch = 0;
			    break;
            }
        }
    }

	if (nameMatch < 1){
		syscall(0, "Bad command!\r\n");
		return;
	}

    nameSum = nameNum;
	
	for (nameNum = nameSum; nameNum < nameSum + 6; nameNum++){
        if (name[nameNum] == '\0' || name[nameNum] == 0xa){
                break;            
        }
		cmdName[nameNum - nameSum] = name[nameNum];
	}
    for (nameNum = nameNum; nameNum < nameSum + 6; nameNum++){
        cmdName[nameNum - nameSum] = '\0';        
    }

    // reusing these variables once more
    nameNum = nameNum + 1;
    nameSum = nameNum;
    
    if (nameMatch == 5){
        for (nameNum = nameNum; nameNum < nameSum + 6; nameNum++){
            if (name[nameNum] == '\0' || name[nameNum] == 0xa){
                break;            
            }
            copyName[nameNum - nameSum] = name[nameNum]; 
        }

        for (nameNum = nameNum; nameNum < nameSum + 6; nameNum++){
            copyName[nameNum - nameSum] = '\0';        
        }
        copyName[6] = '\0';
    }

/*	
	for (nameSum = nameNum + 1; nameSum < 11; nameSum++){
		cmdName[nameSum - (nameNum + 1)] = name[nameSum];
		if (name[nameSum] == '\0'){
			break;
		}
	}
*/

	if (nameMatch == 1){
        //syscall(0, cmdName);
		syscall(3, cmdName, cBuffer, &sectorsRead);
		if (sectorsRead < 1){
            //syscall(0, cmdName);
			syscall(0, "Er: File not found\r\n");
			return;
		} else {
			syscall(0, cBuffer);
			syscall(0, "\r\n");
			return;
		}
	}

	if (nameMatch == 2){
		syscall(3, cmdName, cBuffer, &sectorsRead);
		if (sectorsRead < 1){
			syscall(0, "Er: File not found\r\n");
			return;
		} else {
			syscall(4, cmdName);
			return;
		}
	}

	//syscall(0, "Found the file!\r\n");

	if (nameMatch == 3){
		syscall(2, dir, 2);
		//syscall(0, 'a'); testing purps only, printstring does not
		//work with single characters, reads past array
		for (fileEntry = 0; fileEntry < 512; fileEntry = fileEntry + 32){
			if (dir[fileEntry] == '\0'){
				continue;
			}

			for (entryByte = 0; entryByte < 6; entryByte++){

				outChars[entryByte] = dir[fileEntry + entryByte];

				if (entryByte == 5){
					outChars[6] = '\0';
					syscall(0, outChars);
					syscall(0, "\r\n");
				}
			}
			//bottom is for printing out file sizes w/ each name
			//should move the "newline" func down to here
			/*
			for (entryByte = 6; entryByte < 26; entryByte++){
				if (dir[entryByte] == '\0'){
					(entryByte - 6) + '0'
					break;
				}
			}
			*/
		}
	}

	if (nameMatch == 4){
		syscall(7, cmdName);
	}

    if (nameMatch == 5){
        //syscall(0, copyName);
        syscall(3, cmdName, cBuffer, &sectorsRead);
		if (sectorsRead < 1){
			syscall(0, "Er: File not found.\r\n");
			return;
		} else {
            syscall(0, "File copied.\r\n");
			syscall(8, cBuffer, copyName, sectorsRead);
			return;
		}        
    }

    if (nameMatch == 6){
        inputI = 0;

        while (inputI < 13312){
            //syscall(0, "Flag!\r\n");            
            syscall(1, inputText);
            if (inputText[0] == 0xa){
                break;            
            }
            for (a = 0; a < 79; a++){

                if (inputText[a] == '\0'){
                      cBuffer[a+inputI - 1] = 0xa;
                      cBuffer[a+inputI] = 0xd;
                      inputI++;
                       break;
                }
                cBuffer[a + inputI] = inputText[a];
                if(a == 78){
                      cBuffer[a+inputI - 1] = 0xa;
                      cBuffer[a+inputI] = 0xd;
                      inputI++;
                }
            }
            inputI = inputI + a;
            //syscall(0, "file:\r\n");
            //syscall(0, cBuffer);            
            
        }
        cBuffer[inputI] = '\0';

        sectorsCounted = divideForSectors(inputI);
        
        syscall(8, cBuffer, cmdName, sectorsCounted);

    }	
}


int divideForSectors(int bytes){

    int a;
    int initial;
    int total = 1;

    initial = bytes;
    for (a = 0; a < 26; a++){
        total++;
        if (initial < 512){
            break;
        }
        initial = initial - 512;
    }    
    
    return total; 
}
