/*
	動かすにはHgDisplayerをダウンロードする必要があります。

	スペースインベダーっぽいゲーム
	main関数でそれぞれの処理の関数を呼び出す
	プレーヤー、弾に関する変数は構造体を使い処理
	a,d,wキーが操作キー

*/
#include <stdio.h>
#include <handy.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
doubleLayer lay;
hgevent *key; // 
double size=1.0,size2=2.0,size3=3.0;
int score=0;//スコア：二つの関数で使用するのでグローバル変数
int best_score=0;//ベストスコア：二つの関数で使用するのでグローバル変数

/*プレイヤーに関する構造体*/
typedef struct player{
	double x;//プレイヤーの座標	
	double y;//プレイヤーの座標
	int kill_type0;//タイプ０を倒した数
	int kill_type1;//タイプ１を倒した数
	int kill_type2;//タイプ２を倒した数
	int kill_total;//倒した数の合計
}PLAYER;

/*弾に関する構造体*/
typedef struct attack {
	int attack_done;//攻撃するときの判定
	int attack_count;//攻撃のカウント
	double x_Bullet;//弾の座標
	double y_Bullet;
}ATTACK;

/*タイトル画面************************/
void game_TITLE(){
	int i;
	HgSetColor(HG_YELLOW);
    HgSetFont(HG_T, 20);
    HgBox(140, 352, 20, 20);
    HgText(140,350,"W");
    HgText(110,320,"ATTACK");
    HgBox(62, 262, 20, 20);
    HgText(65,260,"A");
    HgText(65,230,"←");
    HgBox(213, 262, 20, 20);
    HgText(215,260,"D");
    HgText(213,230,"→");
    HgBox(30, 220, 240, 160);
    HgText(35,45,"PUSH THE KEY TO START");
    for(i=0;i<3;i++){
		if(i==0){
			HgSetFillColor(HG_WHITE);
		}
		if(i==1){
			HgSetFillColor(HG_BLUE);
		}
		if(i==2){
			HgSetFillColor(HG_RED);
		}
		HgBoxFill(40,180-i*40,7,4,0 );
		HgBoxFill(40-size, 180-i*40+size, size, size2,0);
		HgBoxFill(40+7, 180-i*40+size, size, size2,0);
		HgBoxFill(40-size2, 180-i*40-size, size, size3,0);
		HgBoxFill(40+8, 180-i*40-size, size, size3,0);
		HgBoxFill(40, 180-i*40-size, size, size,0);
		HgBoxFill(40+6, 180-i*40-size, size, size, 0);
		HgBoxFill(40+size, 180-i*40-size2, size2, size, 0);
		HgBoxFill(40+4, 180-i*40-size2, size2, size, 0);
		HgBoxFill(40, 180-i*40+5, size, size, 0);
		HgBoxFill(40+6, 180-i*40+5, size, size, 0);
		HgBoxFill(40+size, 180-i*40+4, size, size, 0);
		HgBoxFill(40+5, 180-i*40+4, size, size, 0);
	}
	HgSetFont(HG_T, 10);
	HgText(55,175,"THEY ATTACK WHEN YOU ARE UNDER THEM");
	HgText(55,135,"THEY ATTACK YOUR RIGHT SIDE AND LEFT SIDE");
	HgText(55,95,"THEY ATTACK WHEN YOU ATTACK");
	HgBox(30,40, 245,30);
	
	HgGetChar();
}

/*ゲームオーバーのアニメーション*************************************/
void game_OVER(){
	for(int i=0;i<3;i++){
		HgSetFillColor(HG_YELLOW);
		HgBoxFill(0,0,300,400,0);
		HgSleep(0.5);
		HgSetFillColor(HG_BLACK);
		HgBoxFill(0,0,300,400,0);
		HgSleep(0.5);
	}
}

/*ゲームオーバー時のスコア画面*****************************************/
void gmae_OVER_SCREEN(int score){
	if(score>best_score){
		best_score=score;
	}
	HgSetFont(HG_T, 30);
	HgSetColor(HG_YELLOW);
	HgText(60,300,"GAMEOVER");
	HgSleep(0.9);
	HgText(35,200,"YOUR SCORE : %d",score);
	HgSleep(0.9);
	HgSetFont(HG_T, 15);
	HgText(75,150,"YOUR BEST SCORE : %d",best_score);
	HgSleep(0.9);
	HgText(41,85,"PUSH THE KEY TO CONTINUE");
	HgBox(38,82, 215,28);
	HgSleep(0.5);
	HgGetChar();
}

/*ゲーム本編**********************************************************/
void game_START(){
	//構造体の初期化
	PLAYER player = {10.0,10.0,0,0,0,0};
	ATTACK attack = {};
	ATTACK attack_type0[8][10]={};
	ATTACK attack_type1[8][10]={};
	ATTACK attack_type2[8][10]={};
	srand(time(NULL));
   	lay= HgWAddDoubleLayer(0);//レイヤーを配置する
   	int type[8]={};//敵の種類
   	int youLOSE=0;
	int i=0,j=0;//カウンター	
	double xA[20]={},yA=200.0;//初期座標
	int dire=0;//向きのフラグ
	double x_Ariens[10][10]={};//敵の位置
	double y_Ariens[20]={};//敵の位置
	double copyx_Ariens[10][10]={};//敵の位置をもう一度格納する
	int attack_magnification=1;
	int draw[10][10]={};//敵を描画するかどうか
	//y座標を決める
	for(i=0;i<10;i++){
		y_Ariens[i]=yA;
		yA+=20;
	}
	//敵の種類を決める　3:2:2の割合で決める
	for(i=0;i<8;i++){
		type[i]=rand()%7;
	}
	/***ベストスコアの表示********************************************/
	HgSetColor(HG_YELLOW);
    HgSetFont(HG_T, 15);
    HgText(180,350,"BEST SCORE : %d",best_score);
	/***倒したタイプを表示********************************************/
	for(i=0;i<3;i++){
		if(i==0){
			HgSetFillColor(HG_WHITE);
		}else if(i==1){
			HgSetFillColor(HG_BLUE);
		}else{
			HgSetFillColor(HG_RED);
		}
		HgBoxFill(20+i*30,380,7,4,0 );
		HgBoxFill(20+i*30-size, 380+size, size, size2,0);
		HgBoxFill(20+i*30+7, 380+size, size, size2,0);
		HgBoxFill(20+i*30-size2, 380-size, size, size3,0);
		HgBoxFill(20+i*30+8, 380-size, size, size3,0);
		HgBoxFill(20+i*30, 380-size, size, size,0);
		HgBoxFill(20+i*30+6, 380-size, size, size, 0);
		HgBoxFill(20+i*30+size, 380-size2, size2, size, 0);
		HgBoxFill(20+i*30+4, 380-size2, size2, size, 0);
		HgBoxFill(20+i*30, 380+5, size, size, 0);
		HgBoxFill(20+i*30+6, 380+5, size, size, 0);
		HgBoxFill(20+i*30+size, 380+4, size, size, 0);
		HgBoxFill(20+i*30+5, 380+4, size, size, 0);
		}
/*メイン処理******************************************************/
	for(;;){
		int space = HgLSwitch(&lay);//spaceというレイヤーを設置する
		HgSetEventMask(HG_KEY_DOWN);//!!
    	key = HgEventNonBlocking();//!!
		if (key != NULL){
        	switch (key->ch)
        	{
        	case 'a':
        		if(player.x>0){
        			player.x-=10; 
        		}
            	break;
        	case 'd':
        		if(player.x<290){
        			player.x+=10;
        		}
            	break;
            case 'w':
            	if(attack.attack_done==0){
            		attack.attack_done=1;//攻撃する
            	}
  
        	}
        }
        /***プレーヤーを表示**********************************************/
		HgLClear(space);
		HgWSetFillColor(space,HG_WHITE);
		HgWBoxFill(space,player.x+3,player.y+3,11,5,0);
		HgWBoxFill(space,player.x+4, player.y+5, 9, 4,0);
		HgWBoxFill(space,player.x+6,player.y+6,5,5,0);
		/***弾が３5回進行が上限*******************************************/
		if(attack.attack_count==35){
			attack.attack_count=0;
			attack.attack_done=0;
		}
		/***攻撃する時****************************************************/
		if(attack.attack_done==1){
			if(attack.attack_count==0){
				attack.x_Bullet=player.x;
				attack.y_Bullet=player.y;
			}
			attack.attack_count++;
			HgWSetFillColor(space,HG_WHITE);
			HgWBoxFill(space,attack.x_Bullet+7, attack.y_Bullet+10, 4, 4, 1);
			
		}
		/***敵の攻撃処理**********************************************************/
		for(j=0;j<8;j++){
			for(i=0;i<10;i++){
				if(attack_type0[j][i].attack_count==50){
					attack_type0[j][i].attack_count=0;
					attack_type0[j][i].attack_done=0;
				}
				if(attack_type1[j][i].attack_count==50){
					attack_type1[j][i].attack_count=0;
					attack_type1[j][i].attack_done=0;
				}
				if(attack_type2[j][i].attack_count==50){
					attack_type2[j][i].attack_count=0;
					attack_type2[j][i].attack_done=0;
				}
				if(attack_type0[j][i].attack_done==1){
					if(attack_type0[j][i].attack_count==0){
								attack_type0[j][i].x_Bullet=x_Ariens[j][i];
								attack_type0[j][i].y_Bullet=y_Ariens[j];
					}
					attack_type0[j][i].attack_count++;
					HgWSetFillColor(space,HG_WHITE);
					HgWBoxFill(space,attack_type0[j][i].x_Bullet,
					 attack_type0[j][i].y_Bullet, 2, 8,0);
					attack_type0[j][i].y_Bullet-=8;
					if(attack_type0[j][i].y_Bullet<-1){
						attack_type0[j][i].attack_count=50;
					}
				}
				if(attack_type1[j][i].attack_done==1){
					if(attack_type1[j][i].attack_count==0){
								attack_type1[j][i].x_Bullet=x_Ariens[j][i];
								attack_type1[j][i].y_Bullet=y_Ariens[j];
					}
					attack_type1[j][i].attack_count++;
					HgWSetFillColor(space,HG_BLUE);
					HgWBoxFill(space,attack_type1[j][i].x_Bullet,
					 attack_type1[j][i].y_Bullet, 4, 4,0);
					attack_type1[j][i].y_Bullet-=8;
					if(attack_type1[j][i].y_Bullet<-1){
						attack_type1[j][i].attack_count=50;
					}
				}
				if(attack_type2[j][i].attack_done==1){
					if(attack_type2[j][i].attack_count==0){
								attack_type2[j][i].x_Bullet=x_Ariens[j][i];
								attack_type2[j][i].y_Bullet=y_Ariens[j];
					}
					attack_type2[j][i].attack_count++;
					HgWSetFillColor(space,HG_RED);
					HgWBoxFill(space,attack_type2[j][i].x_Bullet,
					 attack_type2[j][i].y_Bullet, 2, 8,0);
					attack_type2[j][i].y_Bullet-=8;
					if(attack_type2[j][i].y_Bullet<-1){
						attack_type2[j][i].attack_count=50;
					}
				}


			}
		}
		/***ゲームオーバー****************************************************************/
		if(youLOSE==1){
			key=NULL;
			game_OVER();
			break;
		}
		/***弾を上方向に移動させる****************************************************/
		attack.y_Bullet+=8;
		/***敵を倒す判定**************************************************************/
		for(j=0;j<8;j++){
			for(i=0;i<10;i++){
				if( fabs(attack.x_Bullet-x_Ariens[j][i]+4.0)<8.0 
					&& fabs(y_Ariens[j]-attack.y_Bullet)<6.0){
					draw[j][i]=1;
					x_Ariens[j][i]=1000.0;
					attack.attack_count=35;
					attack.y_Bullet=600.0;
					attack.attack_done=0;
					if(type[j]==0 || type[j]==1 ||type[j]==2){
						player.kill_type0++;
					}
					if(type[j]==3 || type[j]==4){
						player.kill_type1++;
					}
					if(type[j]==5 || type[j]==6){
						player.kill_type2++;
					}
					break;
				}
			}
		}
		/***敵の方向転換***************************************************************/
		if(dire==0){
			for(j=0;j<8;j++){
				for(i=0;i<10;i++){
					copyx_Ariens[j][i]=xA[j];

					if(draw[j][i]==0){
						x_Ariens[j][i]=copyx_Ariens[j][i];
					}
					xA[j]+=16.0;
				}
				xA[j]=copyx_Ariens[j][1]-15.73;
			}
		}else if(dire==1){
			for(j=0;j<8;j++){
				for(i=0;i<10;i++){
					copyx_Ariens[j][9-i]=xA[j];
					if(draw[j][9-i]==0){
						x_Ariens[j][9-i]=copyx_Ariens[j][9-i];
					}
					xA[j]-=16.0;
				}
				xA[j]=copyx_Ariens[j][8]+15.73;
			}
		}
		/***敵の跳ね返り判定***********************************************************/
		if(copyx_Ariens[0][9]>290.0){//右端
			dire=1;
			for(i=0;i<8;i++){
				xA[i]=290.0;
				y_Ariens[i]-=15.0;
			}
		}else if(copyx_Ariens[0][0]<0){//左端
			dire=0;
			for(i=0;i<8;i++){
				xA[i]=0.0;
				y_Ariens[i]-=15.0;
			}
		}
		/***負け判定*********************************************************************/
		for(j=0;j<8;j++){
			for(i=0;i<10;i++){
				if(fabs(player.x - attack_type0[j][i].x_Bullet+7)<6.0 
					&& fabs(player.y - attack_type0[j][i].y_Bullet)<3.0){
					youLOSE=1;
				}
				if(fabs(player.x - attack_type1[j][i].x_Bullet+7)<6.0 
					&& fabs(player.y - attack_type1[j][i].y_Bullet)<3.0){
					youLOSE=1;
				}
				if(fabs(player.x - attack_type2[j][i].x_Bullet+7)<6.0 
					&& fabs(player.y - attack_type2[j][i].y_Bullet)<3.0){
					youLOSE=1;
				}
			}
		}
		for(j=0;j<8;j++){
			for(i=0;i<10;i++){
				/***敵の描画*******************************************************************/
				if(draw[j][i]==0){
				if(type[j]==0 ||type[j]==1 || type[j]==2){
					HgWSetFillColor(space,HG_WHITE);
				}
				if(type[j]==3 || type[j]==4){
					HgWSetFillColor(space,HG_BLUE);
				}
				if(type[j]==5 || type[j]==6){
					HgWSetFillColor(space,HG_RED);
				}
					HgWBoxFill(space,x_Ariens[j][i],y_Ariens[j],7,4,0 );
					HgWBoxFill(space,x_Ariens[j][i]-size, y_Ariens[j]+size, size, size2,0);
					HgWBoxFill(space,x_Ariens[j][i]+7, y_Ariens[j]+size, size, size2,0);
					HgWBoxFill(space,x_Ariens[j][i]-size2, y_Ariens[j]-size, size, size3,0);
					HgWBoxFill(space,x_Ariens[j][i]+8, y_Ariens[j]-size, size, size3,0);
					HgWBoxFill(space,x_Ariens[j][i], y_Ariens[j]-size, size, size,0);
					HgWBoxFill(space,x_Ariens[j][i]+6, y_Ariens[j]-size, size, size, 0);
					HgWBoxFill(space,x_Ariens[j][i]+size, y_Ariens[j]-size2, size2, size, 0);
					HgWBoxFill(space,x_Ariens[j][i]+4, y_Ariens[j]-size2, size2, size, 0);
					HgWBoxFill(space,x_Ariens[j][i], y_Ariens[j]+5, size, size, 0);
					HgWBoxFill(space,x_Ariens[j][i]+6, y_Ariens[j]+5, size, size, 0);
					HgWBoxFill(space,x_Ariens[j][i]+size, y_Ariens[j]+4, size, size, 0);
					HgWBoxFill(space,x_Ariens[j][i]+5, y_Ariens[j]+4, size, size, 0);//敵の表示
				}
				/***敵の攻撃実行判定******************************************************************/
				if(type[j]==0 || type[j]==1 || type[j]==2 ){
					if(rand()%(80/attack_magnification)==1){
						if(attack_type0[j][i].attack_done==0){
							if(fabs(player.x-x_Ariens[j][i])<30.0){
								attack_type0[j][i].attack_done=1;
							}
						}
					}
				}
				if(type[j]==3 || type[j]==4){
					if(rand()%(110/attack_magnification)==1){
						if(attack_type1[j][i].attack_done==0){
							if(fabs(player.x-x_Ariens[j][i])>60.0){
								attack_type1[j][i].attack_done=1;
							}
						}
					}
				}
				if(type[j]==5 || type[j]==6){
					if(rand()%(110/attack_magnification)==1){
						if(attack_type2[j][i].attack_done==0){
							if(attack.attack_done==1 && fabs(player.x-x_Ariens[j][i])<50.0){
								attack_type2[j][i].attack_done=1;
							}
						}
					}
				}
			}
		}
		/***画面上のスコア等の描画**********************************/
		HgWSetColor(space,HG_YELLOW);
        HgWSetFont(space,HG_T, 10);
        HgWText(space,30,380,"%d",player.kill_type0);
        HgWText(space,60,380,"%d",player.kill_type1);
        HgWText(space,80,380,"%d",player.kill_type2);
        HgWSetFont(space,HG_T, 20);
        HgWText(space,180,365,"SCORE");
        player.kill_total=player.kill_type0+player.kill_type1+player.kill_type2;
        HgWSetFont(space,HG_T, 30);
        HgWText(space,250,360,"%d",player.kill_total);
		HgSleep(0.02);
		if(player.kill_total==20){
			attack_magnification=5;
		}
		if(player.kill_total==30){
			attack_magnification=10;
		}
	}
	score=player.kill_total;
}

int main(){
	HgOpen(300,400);
	HgSetFillColor(HG_BLACK);
	HgBoxFill(0,0,300,400,0);
	game_TITLE();//!!
	for(;;){
		HgClear();
		HgSetFillColor(HG_BLACK);
		HgBoxFill(0,0,300,400,0);
		game_START();//!!
		HgClear();
		HgSetFillColor(HG_BLACK);
		HgBoxFill(0,0,300,400,0);
		gmae_OVER_SCREEN(score);//!!
	}
	
	
}