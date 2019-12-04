int i=0;
void path (){
  if(j!=0){exit_path();}
  else{i = i + 1;
    //First, go to the known cylinder
    if (i == 1){ turnLeft(); } //at first T
    else if (i == 2){ turnLeft(); } //at cross junction
    //Once we have the first cylinder, go to every corner of the maze
    else if (i == 3){ turnLeft(); } //at cross junction
    else if (i == 4){ foward(); } //at T junction
    //Corner found and go back to the previous junction 
    else if (i == 5){ turnLeft(); } //at T junction
    else if (i == 6){ turnLeft(); } //at T junction
    //Corner found and go back to the previous junction
    else if (i == 7){ foward(); } //at T junction
    else if (i == 8){ foward(); } //at T junction
    else if (i == 9){ turnLeft(); } //at cross junction
    else if (i == 10){ foward(); } //at T junction
    else if (i == 11){ turnLeft(); } //at cross junction
    //Corner found and go back to the previous junction
    else if (i == 12){ foward(); } //at cross junction avoiding the island
    //Last corner found and go back to the previous junction
    else if (i == 13){ turnLeft(); } //at cross junction
    else if (i == 14){ turnLeft(); } //at T junction
    //Exit found
   }
}

//When the three cylinders have been found find the fastest way to the exit

int j=0;
void exit_path(){
  if(i==3){
    if (j==0){reverse();}
    j = j + 1;
    if (j == 1){ forward(); } //at first cross
    else if (j == 2){ turnRight(); } //at T junction
    //Exit found
    }
  if(i==4){
    j = j + 1;
    if (j == 1){ turnLeft(); } //at first T
    else if (j == 2){ turnRight(); } //at T junction
    else if (j == 3){ turnLeft(); } //at cross junction
    else if (j == 4){ forward(); } //at cross junction
    else if (j == 5){ turnLeft(); } //at T junction
    //Exit found  
    }
  if(i==5){
    j = j + 1;
    if (j == 1){ turnRight(); } //at T junction
    else if (j == 2){ turnLeft(); } //at cross junction
    else if (j == 3){ forward(); } //at cross junction
    else if (j == 4){ turnLeft(); } //at T junction
    //Exit found  
    }
  if(i==6){
    if (j==0){reverse();}
    j = j + 1;
    if (j == 1){ forward(); } //at T junction
    else if (j == 2){ turnLeft(); } //at cross junction
    else if (j == 3){ forward(); } //at cross junction
    else if (j == 4){ turnLeft(); } //at T junction
    //Exit found
    }
  if(i==7){
    j = j + 1;
    if (j == 1){ turnLeft(); } //at cross junction
    else if (j == 2){ forward(); } //at cross junction
    else if (j == 3){ turnLeft(); } //at T junction
    //Exit found
    }
  if(i==8){
    j = j + 1;
    if (j == 1){ turnLeft(); } //at cross junction
    else if (j == 2){ turnRight(); } //at T junction
    //Exit found
    }
  if(i==10){
    if (j==0){reverse();}
    j = j + 1;
    if (j == 1){ turnLeft(); } //at T junction
    //Exit found
    }
  if(i==11){
    if (j==0){reverse();}
    j = j + 1;
    if (j == 1){ turnRight(); } //at cross junction
    else if (j == 2){ turnLeft(); } //at T junction
    //Exit found
    }
  if(i==12){
    if (j==0){reverse();}
    j = j + 1;
    if (j == 1){ turnLeft(); } //at cross junction
    else if (j == 2){ turnLeft(); } //at T junction
    //Exit found
    } 
}
