// Gabe and Bruce's Operating System Kernel
// Following Dr. Margaret Black and Joe Matta's documentation
//  kernel.c

void printChar(char);
void printString(char*);
void readString(char);
void readSector(char*, int);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void readFile(char*, char*, int*);
void charMatch(char*, char*);
void executeProgram(char*);
void terminate();
void writeSector(char*, int);
void deleteFile(char*);
void writeFile(char*, char*, int);

void main() {
	//printString("hello world!");

	//char line[80];
	//printString("Enter a line: ");
	//readString(line);
	//printString(line);

	//char buffer[512];
	//readSector(buffer, 30);
	//printString(buffer);

	//makeInterrupt21();
	//interrupt(0x21,0,0,0,0);

	//char line[80];
	//makeInterrupt21();
	//interrupt(0x21,1,line,0,0);
	//interrupt(0x21,0,line,0,0);

	//char buffer[13312]; /* this is the maximum size of a file */
	//int sectorsRead;
	//makeInterrupt21();
	//interrupt(0x21, 3, "messag", buffer, &sectorsRead); /* read the file into buffer */
	//if(sectorsRead>0){
	//	interrupt(0x21, 0, buffer, 0 ,0); /* print out the file */
	//}
	//else{
	//	interrupt(0x21, 0, "messag not found\r\n", 0, 0); /* no sectors read? then print an error */
	//}

	makeInterrupt21();
	//interrupt(0x21, 4, "tstpr2",0,0);
    //interrupt(0x21, 8,"this is a test message","testmg",3); <- WORKS HAHAHAHAHAHA
	interrupt(0x21, 4, "shell",0,0);
	//to prevent crashing
	while(1);
}


void printChar(char chars){
	interrupt(0x10, 0xe*256+chars,0,0,0);
}

void printString(char* chars){
	while(*chars != 0x0){
		interrupt(0x10, 0xe*256+*chars, 0, 0, 0);
		chars++;
	}
}

void readString(char chars[80]){
	int counter = 0;
	while(1){
		char x = interrupt(0x16,0,0,0,0);
		if(x == 0xd){
			printChar(0xd);
			printChar(0xa);
			chars[counter] = 0xa;
			counter++;
			chars[counter] = 0x0;
			break;
		}
		if(x == 0x8){
			if(counter > 0){
				counter--;
				printChar(0x8);
				printChar(0x20);
				//replace previous letter with a space
				chars[counter] = 0x20;
				printChar(0x8);
			}
			continue;
		}
		printChar(x);
		chars[counter] = x;
		counter++;
	}
}

void readSector(char* buffer, int sector){
	int ah = 2; //read, not write to, sector
	int al = 1; //num of sectors to use
	int ch = 0; //track number
	int cl = sector + 1; //relative sector number
	int dh = 0; //head number
	int dl = 0x80; //hard disk
	interrupt(0x13, ah*256 + al, buffer, ch*256+cl, dh*256+dl);
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
	switch(ax){
		case 0:
			printString(bx);
		break;

		case 1:
			readString(bx);
		break;

		case 2:
			readSector(bx, cx);
		break;

		case 3:
			readFile(bx, cx, dx);
		break;

		case 4:
			executeProgram(bx);
		break;

		case 5:
			terminate();
		break;

		case 6:
			writeSector(bx, cx);
		break;

        case 7:
            deleteFile(bx);
        break;

        case 8:
            writeFile(bx, cx, dx);
        break;

		default:
		printString("Incorrect AX code was used in interrupt21");
		break;
	}
	//printString("Hello, world!");
}

void writeSector(char* address, int sector){
	int ah = 3; //write, not read to, sector
	int al = 1; //num of sectors to use
	int ch = 0; //track number
	int cl = sector + 1; //relative sector number
	int dh = 0; //head number
	int dl = 0x80; //hard disk
	interrupt(0x13, ah*256 + al, address, ch*256+cl, dh*256+dl);
}

// For development speed, going to assume for now
// both arrays are not shorter than 6 chars

void readFile(char* fileName, char* b, int* s){
	char dir[512];
	int found = 0;
	int fEntry = 0;
	int a = 0;
	int sector;

	*s = 0;
	readSector(dir, 2);

	for (fEntry = 0; fEntry < 512; fEntry = fEntry + 32){
		found = 1;
		for (a = 0; a < 6; a++){
			if (dir[fEntry + a] != fileName[a]){
				found = 0;
				break;
			}
			if (dir[fEntry + a] == '\0' && fileName[a] == '\0'){				
                break;
			}
		}

		if (found == 1){
			break;
		}
	}

	if (found == 0){
        // printString("File not found");
		return;
	}

	for (sector = 6; sector < 26; sector++){
		if (dir[fEntry + sector] == '\0'){
			break;
		}
		readSector(b, dir[fEntry + sector]);
		b = b + 512;
		*s = *s + 1;
	}
}


void executeProgram(char* name){
	char buffer[13312];
	int sectorsRead;
    int a;

	readFile(name, buffer, sectorsRead);
	for (a = 0; a < 13312; a++)
	{
		putInMemory(0x2000, a, buffer[a]);
	}
	launchProgram(0x2000);
}

void terminate(){
	// while(1);
	char shellname[6];
	shellname[0] = 's';
	shellname[1] = 'h';
	shellname[2] = 'e';
	shellname[3] = 'l';
	shellname[4] = 'l';
	shellname[5] = '\0';
	executeProgram(shellname);
}

void deleteFile(char* filename){
    char map[512];
    char dir[512];
    int sectorsRead;
    int found = 0;
    int fEntry;
    int a;
    int b;
    
    readSector(map, 1);
    readSector(dir, 2);

    for (fEntry = 0; fEntry < 512; fEntry = fEntry + 32){
	    found = 1;
        for (a = 0; a < 6; a++){
		    if (dir[fEntry + a] != filename[a]){
                // file is not found			    
                found = 0;
                break;
		    }
		    if (dir[fEntry + a] == '\0' && filename[a] == '\0'){
                // end loop if there's no more chars to compare
                break;
		    }
	    }
        
        if (found == 1){
            for (a = 6; a < 32; a++){
                if (dir[fEntry + a] == '\0'){
                    // no more sectors for the file                
                    break;            
                }
                dir[fEntry] = '\0';
                // dunno about this, may break
                b = dir[fEntry + a] - 1;
                map[b] = '\0';
            }

            break;
        }
    }

    writeSector(map, 1);
    writeSector(dir, 2);
}

void writeFile(char* buffer, char* filename, int numberOfSectors){
    char map[512];
    char dir[512];
    char fileStore[512];
    int found;
    int notFull;
    int fEntry;
    int mEntry;
    int a;
    int sectorI;
    int bufferI;

    readSector(map, 1);
    readSector(dir, 2);

    found = 0;
    notFull = 0;
    sectorI = 0;

    for (fEntry = 0; fEntry < 512; fEntry = fEntry + 32){
        if (dir[fEntry] == '\0'){
            found = 1;
            break;
        }
    }

    if (found == 0){
        return;
    }

    for (mEntry = 3; mEntry < 512; mEntry++){
        if (sectorI >= numberOfSectors){
            notFull = 1;
            break;
        }        
        if (map[mEntry] == '\0'){
            // will use this variable later to subtract from
            // mEntry to get exact location of free space
            sectorI++;
        }
    }
    
    if (notFull == 0){
        printString("Error: No free memory in system.");
        return;   
    }
    
    for (a = 0; a < 6; a++){
        if (filename[a] == '\0'){
            // don't want to risk checking out of bounds elements                    
            break;
        }
        dir[fEntry + a] = filename[a];            
    }
    for (a = a; a < 6; a++){
        dir[fEntry + a] = '\0';
    }

    // get the sector offset w/ values found earlier
    mEntry = mEntry - sectorI; 
    
    // reset sectorI to repurpose it
    for (sectorI = 0; sectorI < numberOfSectors; sectorI++){
        // set sector to 0xFF in the map
        map[mEntry + sectorI] = 0xFF;
        // add to file's directory entry
        dir[fEntry + a + sectorI] = (mEntry + sectorI);

        for (bufferI = 0; bufferI < 512; bufferI++){
            if (buffer[(sectorI * 512) + bufferI] == '\0'){
                // don't read out of bounds'
                break;
            }
            fileStore[bufferI] = buffer[(sectorI * 512) + bufferI];
        }
        for (bufferI = bufferI; bufferI < 512; bufferI++){
            fileStore[bufferI] = '\0';
        }

        writeSector(fileStore, mEntry + sectorI);
    }

    // prep to repurpose fEntry and a
    a = fEntry + a + sectorI;
 
    for (fEntry = a; fEntry < 32; fEntry++){
        dir[fEntry] = '\0';
    }       

    writeSector(map, 1);
    writeSector(dir, 2);
}
