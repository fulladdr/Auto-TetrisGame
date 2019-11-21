#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;
	initscr();
	noecho();
	keypad(stdscr, TRUE);	
	createRankList();
	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		case MENU_RANK: rank(); break;
		case MENU_RECOMMENDED : recommendedPlay(); break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;
	DrawOutline();
	DrawField();
	recommend_mine(NULL);
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	move(9,WIDTH+10);
	DrawBox(10,WIDTH+10,4,8);
	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(16,WIDTH+10);
	printw("SCORE");
	DrawBox(17,WIDTH+10,1,8);
}

int GetCommand(){
	int command;

	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(18,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
	for( i = 0; i < 4; i++ ){
		move(11+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[2]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int c[4][4];
	for(int i = 0;i<4;i++){
		for(int j = 0;j<4;j++){
			c[j][i] = block[currentBlock][blockRotate][j][i];
		}
	}
	int ch = 0;
	for(int i =0;i<4;i++){
		for(int j = 0;j<4;j++){
			int x = blockX+i;
			int y = blockY+j;
			if(c[j][i]==1 && (x<0||y<0)) { return 0;}
			if ( (x<0||y<0)) continue;
			if(c[j][i]==1 && (x<0||x>=10||y>=22)) {ch = 1;break;}
			if(f[y][x]==1&&c[j][i]==1) {ch = 1;break;} 
		}
	}
	if(ch==0) return 1;
	else return 0;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	//DrawField();
	//DrawBlock(blockY,blockX,currentBlock,blockRotate,' ');
	int rotate = blockRotate;
	int x = blockX,y = blockY;
	switch(command){
		case KEY_UP:
			rotate=(rotate+3)%4; break;
		case KEY_DOWN:
			y--;
			break;
		case KEY_RIGHT:
			x--;
			break;
		case KEY_LEFT:
			x++;
			break;
		default : y--;
	}
	int sy = y;
	while(CheckToMove(field,currentBlock,rotate,sy,x)) sy++;
	sy--;
	for(int i = 0;i<4;i++){
		for(int j = 0;j<4;j++){
			if(block[currentBlock][rotate][i][j]==1){
				if(i+y>=0){
					move(i+y+1,j+x+1);
					printw(".");
				}
				if(i+sy>=0){
					move(i+sy+1,j+x+1);
					printw(".");
				}
			}
		}
	}
	DrawBlockWithFeatures(blockY,blockX,currentBlock,blockRotate);
	move(HEIGHT,WIDTH+10);
}

void BlockDown(int sig){
	// user code
	int t =CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX); 
	if(!t&&blockY==-1){
		gameOver = 1;
	}
	else if(!t&&blockY!=-1){
		int cnt = AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		int delete = DeleteLine(field);
		score += cnt+delete*delete*100;
		PrintScore(score);
		DrawField();
		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand()%7;
		blockY=-1;
		DrawNextBlock(nextBlock);
		recommend_mine(NULL);
		blockX=WIDTH/2-2;
		timed_out=0;
	}
	else if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)){
		blockY++;
		DrawChange(field,0,nextBlock[0],blockRotate,blockY,blockX);
	}
	timed_out = 0;

}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int k = 0;
	for(int i = 0;i<4;i++){
		for(int j = 0;j<4;j++){
			int y = blockY + i;
			int x = blockX + j;
			if(block[currentBlock][blockRotate][i][j]==1){
				f[y][x] = 1;
				if(y+1==HEIGHT||f[y+1][x]==1) k++;
			}
		}
	}
	return k*10;
	
	
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int cnt = 0;
	for(int i = 0;i<HEIGHT;i++){
		int t = 0;
		for(int j = 0;j<WIDTH;j++){
			if(f[i][j]==1) t++;
		}
		if(t==WIDTH){
			cnt++;
			for(int j = i;j>0;j--){
				for(int k = 0;k<WIDTH;k++){
					f[j][k]=f[j-1][k];
				}
			for(int k = 0;k<WIDTH;k++) f[0][k] = 0;
			}		
		}
	}
	return cnt;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	while(CheckToMove(field,blockID,blockRotate,y,x)) y++;
	y--;
	DrawBlock(y,x,blockID,blockRotate,'/');
}
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
	DrawBlock(y,x,blockID,blockRotate,' ');
	DrawShadow(y,x,blockID,blockRotate);
	
	DrawRecommend(Maxy,Maxx,nextBlock[0],Maxrotate);
}
void createRankList(){
	// user code
	FILE* fp = fopen("rank.txt","r");
	int n; fscanf(fp,"%d",&n);
	for(int i = 0;i<n;i++){
		char* x;
		int xx;
		fscanf(fp,"%s",name[i]);
		fscanf(fp,"%d",&xx);
		arr[i] = xx;
	}
	fclose(fp);
	arrsize = n;
}

void rank(){
	// user code
	int from,to;
	int n=arrsize;
	clear();
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	switch(wgetch(stdscr)){
		case '1':
			from = -1;
			to = -1;
			echo();
			printw("X: ");
			scanw("%d",&from);
			printw("Y: ");
			scanw("%d",&to);
			if(from==-1) from = 1;
			if(to==-1) to = n;
			
			printw("          name      |      score           \n");
			printw("-------------------------------------------\n");
			
			for(int i = from-1;i<=to-1;i++){
				if(i>=n) break;
				if(i<0) continue;
				printw("%-20s|",name[i]);
				printw("%-10d\n",arr[i]);
			}
			getch();

			break;
			noecho();
		case '2':
			echo();
			char arr2[200];
			for(int i = 0;i<200;i++) arr2[i] = 0;
			printw("your name: ");
			scanw("%s",arr2);
			int i=0;
			int j;
			for(j=0;j<n;j++){
				if(strcmp(name[j],arr2)==0) break;
				if(name[j][0]==0&&arr2[0]==0) break;
			}
			if(j==n){
				printw("Search failure: no name in the list\n");getch(); break;
			}
			noecho();
			printw("          name      |      score           \n");
			printw("-------------------------------------------\n");
			printw("%-20s|%-10d\n",name[j],arr[j]);
			writeRankFile();
			getch();
			break;
		case '3':
			echo();
			int x;
			printw("input the rank: ");
			scanw("%d",&x);
			if(x>arrsize) printw("Search failure : The rank not in list\n");
			else{
			x-=1;
			for(int i = x;i<arrsize;i++){
				arr[i] = arr[i+1];
				strcpy(name[i],name[i+1]);
			}
			arrsize -=1;
			printw("result : the rank deleted\n");
			writeRankFile();
			noecho();
			}
		getch();

		
	}
}

void writeRankFile(){
	// user code
	FILE* fp = fopen("rank.txt","w");
	fprintf(fp,"%d\n",arrsize);
	for(int i = 0;i<arrsize;i++){
		fprintf(fp,"%s %d\n",name[i],arr[i]);
	}
}

void newRank(int score){
	// user code
	clear();
	arr[arrsize] = score;
	char arr2[400];
	printw("your name: ");
	echo();
	scanw(" %s",name[arrsize]);
	noecho();
	arrsize++;
	for(int i = 0;i<arrsize;i++){
		for(int j = i;j<arrsize;j++){
			if(arr[i]<arr[j]){
				int t = arr[j];
				arr[j] = arr[i];
				arr[i] = t;
				int x = strlen(name[i])+1;
				int y = strlen(name[j])+1;
				for(int k = 0;k<x;k++){
					arr2[k] = name[i][k];
				}
				for(int k = 0;k<x;k++){
					name[i][k] = name[j][k];
				}
				for(int k = 0;k<x;k++){
					name[j][k] = arr2[k];
				}
				
			}
		}
	}
	writeRankFile();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
	DrawBlock(y,x,nextBlock[0],Maxrotate,'R');
}

int recommend(RecNode *root){
	
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
	if(root==NULL){
		root=(RecNode*)calloc(sizeof(RecNode),1);
		root->score = 0;
		for(int i = 0;i<HEIGHT;i++){
			for(int j = 0;j<WIDTH;j++){
				root->f[i][j] = field[i][j];
			}
		}
	}
	int maxx=0,maxy=0;
	int maxrotate=0;
	/*level이 3이 되면 리턴*/
	if(root->lv==3) return 0;
	int child = 0;
	for(int i = 0;i<4;i++){
		/*각각의 rotate에 대해서*/
		int rotate = i;
		for(int j = -4;j<WIDTH+4;j++){
			/*y를 내릴 수 있을 때까지 내린다*/
			int y = -1;
			if(!CheckToMove(root->f,nextBlock[root->lv],rotate,y,j)) continue;
			while(CheckToMove(root->f,nextBlock[root->lv],rotate,y,j)) y++;
			y--;
			root->c[child] = (RecNode*)calloc(sizeof(RecNode),1);
			for(int a = 0;a<HEIGHT;a++){
				for(int b = 0;b<WIDTH;b++){
					root->c[child]->f[a][b] = root->f[a][b];
				}
			}
			/*점수 계산*/
			int cnt = AddBlockToField(root->c[child]->f,nextBlock[root->lv],rotate,y,j);
			int delete = DeleteLine(root->c[child]->f);
			root->score=cnt+delete*delete*100;
			root->c[child]->lv = (root->lv)+1;
			root->c[child]->score = 0;
			int k = recommend(root->c[child]);
			/*점수가 max보다 클때만 기록*/
			if(max<k+(root->score)) {max = k+(root->score);maxx=j;maxy=y;maxrotate=rotate;}
			child++;
		}
	}
	/*level이 0일때 표시*/
	if(root->lv==0){
		Maxx=maxx;
		Maxy=maxy;
		Maxrotate = maxrotate;
	}
	return max;
}

int recommend_mine(RecNode *root){
	int max=-2000000000;
	if(root==NULL){
		root=(RecNode*)calloc(sizeof(RecNode),1);
		root->score = 0;
		for(int i = 0;i<HEIGHT;i++){
			for(int j = 0;j<WIDTH;j++){
				root->f[i][j] = field[i][j];
			}
		}
	}
	int maxx=0,maxy=0;
	int maxrotate=0;
	int h[40]; 
	int hh[40];
	int sumf = 0;
	for(int i = 0;i<WIDTH;i++) hh[i] = 2000000000;
	for(int a = 0;a<HEIGHT;a++){
			for(int b = 0;b<WIDTH;b++){
				if(root->f[a][b]==1&&hh[b]>a) hh[b] = a;
		}
	}
	for(int b = 0;b<WIDTH;b++) if(hh[b]==2000000000) hh[b] = HEIGHT;
	for(int b = 0;b<WIDTH;b++) sumf+=hh[b];

	if(root->lv==3) return 0;
	int child = 0;
	for(int i = 0;i<4;i++){
		int rotate = i;
		for(int j = -4;j<WIDTH+4;j++){
			int y = -1;
			for(int i = 0;i<40;i++) h[i] = 2000000000; 
			if(!CheckToMove(root->f,nextBlock[root->lv],rotate,y,j)) continue;
			while(CheckToMove(root->f,nextBlock[root->lv],rotate,y,j)) y++;
			y--;
			root->c[child] = (RecNode*)calloc(sizeof(RecNode),1);
			for(int a = 0;a<HEIGHT;a++){
				for(int b = 0;b<WIDTH;b++){
					root->c[child]->f[a][b] = root->f[a][b];
				}
			}
			int cnt = AddBlockToField(root->c[child]->f,nextBlock[root->lv],rotate,y,j);
			int delete = DeleteLine(root->c[child]->f);
			int sumh = 0;
			int hol = 0;
			for(int a = 0;a<HEIGHT;a++){
				for(int b = 0;b<WIDTH;b++){
					if(root->c[child]->f[a][b]==1&&h[b]>a) h[b] = a;
				}
			}
			for(int a = 0;a<HEIGHT;a++){
				for(int b = 0;b<WIDTH;b++){
					if(a>h[b]&&root->c[child]->f[a][b]==0) hol++;
				}
			}
			int diffx = -2000000000;
			int diffy = 2000000000;
			for(int b = 0;b<WIDTH;b++){ 
				if(h[b]==2000000000) h[b] = HEIGHT;
			}
			for(int b = 0;b<WIDTH;b++) sumh+=h[b];
			for(int b = 0;b<WIDTH-1;b++){
				int xxx;
				if(h[b]<h[b+1]) xxx = h[b+1];
				else xxx = h[b];
				if(diffx<xxx) diffx = xxx; 
			} 
			for(int b = 0;b<WIDTH;b++){
				if(diffy>h[b]) diffy= h[b];
			}
			sumh /= WIDTH;
			int diff = 0;
			for(int b = 0;b<WIDTH;b++) diff += sumh>h[b]?(sumh-h[b]):h[b]-sumh;
			root->score= cnt*4+delete*delete*400-diff*30-hol*hol+(sumh-sumf)*10+diffy*5;
			root->c[child]->lv = (root->lv)+1;

			root->c[child]->score = 0;
			int k = recommend_mine(root->c[child]);
			if(max<k+(root->score)) {max = k+(root->score);maxx=j;maxy=y;maxrotate=rotate;}	
			child++;
		}
	}
	for(int i = 0;i<child-1;i++) {
		if(root->c[i]!=NULL) {free(root->c[i]); root->c[i] = NULL;}
	}
	if(root->lv==0){
		Maxx=maxx;
		Maxy=maxy;
		Maxrotate = maxrotate;
	}	
		return max;
}


void recommendedPlay(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		refresh();
		RecNode* head = NULL;
		recommend_mine(head);
		if(head!=NULL) {free(head);head = NULL;}
		DrawRecommend(Maxy,Maxx,nextBlock[0],Maxrotate);
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}
		int cnt = AddBlockToField(field,nextBlock[0],Maxrotate,Maxy,Maxx);
		int delete = DeleteLine(field);
		score += cnt + delete*delete*100;
		PrintScore(score);
		DrawField();
		nextBlock[0] = nextBlock[1];
		nextBlock[1] = nextBlock[2];
		nextBlock[2] = rand()%7;
		blockY=-1;
		blockRotate = Maxrotate;
		DrawNextBlock(nextBlock);
		blockX=WIDTH/2-2;
		int t = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX);
		if(!t){
			gameOver =1;
		}
		timed_out = 0;
	}while(!gameOver);

	alarm(0);
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
}
