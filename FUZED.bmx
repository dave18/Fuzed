Strict

Framework BRL.D3D7Max2D
Import BRL.GLMax2D
Import BRL.System
Import BRL.FileSystem
Import BRL.FreeAudioAudio
Import BRL.WAVLoader
Import BRL.PNGLoader
Import BRL.JPGLoader
Import BRL.BMPLoader
Import BRL.Random
Import Pub.FreeJoy


AppTitle$="FUZED"							' Set application title


'**** Includes ****
Include "bass.bmx"
Include "keydata.bmx"

setkeynames()

Global avetime:Int[]=[5,5,5,5,5,5,5,5,5,5]

'Print BASSMOD_GetVersion()
'Print BASSMOD_GetDeviceDescription(-1)

Global soundpresent=1

If BASSMOD_Init(-1,44100,BASS_DEVICE_NOSYNC)=False Then		'Initialise audio default device
	If BASSMOD_Init(0,44100,BASS_DEVICE_NOSYNC)=False Then		'Initialise audio device 0
		Local e:Int=BASSMOD_ErrorGetCode()
		Notify "Could not initialise audio~nError Code: "+e,True
		soundpresent=0
	End If
End If

Global oldvol:Int
If soundpresent Then oldvol=BASSMOD_GetVolume()			'Store current volume settings

'Collision Layers
Const COL_FOREG=$1			'All foreground tiles
Const COL_LEFTH=$2			'Lefthand boundary foreground tiles
Const COL_RIGHTH=$4			'Righthand boundary foreground tiles
Const COL_ENEMY=$8			'Enemy sprites
Const COL_BOMB=$10			'Bombs
Const COL_EXPLOSION=$20		'Explosions
Const COL_COIN=$40			'Coins
Const COL_GEM=$80				'Gems
Const COL_PLAYER=$100			'Players
Const COL_DOOR=$200			'Door (Exit)
Const COL_MINE=$400			'Mines
Const COL_PICKUP=$800			'Pick ups
Const COL_ROOF=$1000			'Ceilings
Const COL_DESTROY_TILE=$2000 	'Destructable Background Tiles


Rem
****
Level Bank format (5032 bytes)
-----------------

HEADER
======
Data									Offset			Size
----									------			----
number of levels						0				Int
level size							4				Int

LEVELDATA
=========
Player 1 x pos						0				Int
Player 1 y pos						4				Int
player 2 x pos						8				Int
player 2 y pos						12				Int
Number of enemies						16				Int
enemy 1 x pos							20				Int
enemy 1 y pos							24				Int
enemy 1 dir							28				Int
enemy 1 type							32				Int
enemy 1 active						36				Int
..... continue enemies (room for 8) ......
exit door x pos						180				int
exit door y pos						184				int
10 spare ints							188 - 227		Int
level background number				228				Int
layer 0 tile data						232				Byte
layer 1 tile data						1432				Byte
layer 2 tile data						2632				Byte
tile boundary data					3832 to 5031		Byte
****
End Rem

Global pleft[2],pright[2],pup[2],pdown[2],pfire[2],pjump[2],pcycle[2],pausekey

'**** DEFAULT KEY ASSIGNMENTS ****
pleft[0]=KEY_Z
pright[0]=KEY_X
pup[0]=0
pdown[0]=0
pjump[0]=KEY_F
pfire[0]=KEY_C
pcycle[0]=KEY_V

pleft[1]=KEY_NUM1
pright[1]=KEY_NUM2
pup[1]=0
pdown[1]=0
pjump[1]=KEY_NUM9
pfire[1]=KEY_NUM6
pcycle[1]=KEY_NUM3

Global hiscore:Int[100],hiname:String[100],hinew:Byte[100]

'**** Default Hi Scores ****
For Local z:Int=0 To 99
	hiscore[z]=10000-z*100
	hinew[z]=0
	If z & 1 Then hiname[z]="ED!" Else hiname[z]="FUZ"
Next

pausekey=KEY_P

Global GfxMode=0
Global background_on=1

If FileType("fuzed.cfg")=1 Then LoadSettings()
If FileType("fuzed.hi")=1 Then LoadScores()

SetGfxMode()


'**** Load level data ****
Global levelbank:TBank= LoadBank ("LEVELDATA.DAT")
'Global levelbank:TBank= LoadBank ("test2.DAT")
Global LEVEL_SIZE
Global NUMBER_LEVELS

NUMBER_LEVELS=PeekInt(levelbank,0)
LEVEL_SIZE=PeekInt(levelbank,4)


'**** Load music modules ****
Global tune0:TBank=LoadBank("audio\FuZED.title.Main.xm")
Global tune0add:Byte Ptr=BankBuf(tune0)
Global tune1:TBank=LoadBank("audio\FuZED.lev.Checkers.xm")
Global tune1add:Byte Ptr=BankBuf(tune1)
Global tune2:TBank=LoadBank("audio\FuZED.lev.Ancient.xm")
Global tune2add:Byte Ptr=BankBuf(tune2)
Global tune3:TBank=LoadBank("audio\FuZED.lev.Ancient.xm")
Global tune3add:Byte Ptr=BankBuf(tune3)
Global tune4:TBank=LoadBank("audio\FuZED.lev.Ancient.xm")
Global tune4add:Byte Ptr=BankBuf(tune4)


'**** Load Samples ****
Global explosion_snd:TSound=LoadSound("audio\explosion_x.wav")
Global collect_snd:TSound=LoadSound("audio\sound005.wav")
Global jump_snd:TSound=LoadSound("audio\sound008.wav")
Global bomb_land_snd:TSound=LoadSound("audio\hammer.wav")
Global mine_arm_snd:TSound=LoadSound("audio\blips4.wav")


'*** Load Intro GFX ***
SetMaskColor 255,0,255
'Global intro_clouds :TImage = loadimage("graphics\cloud.jpg")
'Global intro_floor :TImage = loadimage("graphics\floor.png")
'Global intro_mountain :TImage = loadimage("graphics\mountain.jpg")
Global intro_header:TImage = LoadImage("graphics\page_header.png")
Global intro_footer:TImage = LoadImage("graphics\page_footer.png")
Global intro_screen_scroll:TImage = LoadImage ("graphics\page_background_1280_scroll_wrap.jpg")
Global intro_screen_static:TImage = LoadImage ("graphics\page_background_640.jpg")
Global mouse_pointer:TImage = LoadImage ("graphics\mouse_pointer.png")
Global intro_icons:TImage = LoadAnimImage ("graphics\intro_icons.png",96,96,0,8)



'**** Load Tiles ****
SetMaskColor 255,0,255
Global tilesBitmap :TImage = LoadAnimImage ("LEVELDATA.bmp", 16, 16, 0, 240)
'Global backscreen0:TImage = loadimage ("graphics\lev01_bkg0.png")
'Global backscreen1:TImage = loadimage ("graphics\lev01_bkg1.png")
Global backscreen0:TImage = LoadImage ("graphics\area01_bkg0-test.png")
Global backscreen1:TImage = LoadImage ("graphics\area01_bkg1-test.png")

Global font:TImage = LoadAnimImage("graphics\16x16_font.png",16,16,0,59)
Global smallfont:TImage = LoadAnimImage("graphics\8x8_font.png",8,8,0,10)

AutoMidHandle(True)

'**** Load sprites ****
SetMaskColor 255,0,255
Global snipe_stand_left:TImage = LoadAnimImage("graphics\snipe.stand_left.png",32,32,0,1)
Global snipe_stand_right:TImage = LoadAnimImage("graphics\snipe.stand_right.png",32,32,0,1)
Global snipe_run_left:TImage = LoadAnimImage("graphics\snipe.run_left.png",32,32,0,8)
Global snipe_run_right:TImage = LoadAnimImage("graphics\snipe.run_right.png",32,32,0,8)
Global snipe_skid_halt_left:TImage = LoadAnimImage("graphics\snipe.skid_halt_left.png",32,32,0,4)
Global snipe_skid_halt_right:TImage = LoadAnimImage("graphics\snipe.skid_halt_right.png",32,32,0,4)
Global snipe_turn_left_to_right:TImage = LoadAnimImage("graphics\snipe.turn_left_to_right.png",32,32,0,4)
Global snipe_turn_right_to_left:TImage = LoadAnimImage("graphics\snipe.turn_right_to_left.png",32,32,0,4)
Global snipe_blink_facing_left:TImage = LoadAnimImage("graphics\snipe.blink_facing_left.png",32,32,0,8)
Global snipe_blink_facing_right:TImage = LoadAnimImage("graphics\snipe.blink_facing_right.png",32,32,0,8)
Global snipe_facing_left_drop_bomb:TImage = LoadAnimImage("graphics\snipe.facing_left_drop_bomb.png",32,32,0,4)
Global snipe_facing_right_drop_bomb:TImage = LoadAnimImage("graphics\snipe.facing_right_drop_bomb.png",32,32,0,4)
Global snipe_facing_left_drop_mine:TImage = LoadAnimImage("graphics\snipe.facing_left_drop_mine.png",32,32,0,4)
Global snipe_facing_right_drop_mine:TImage = LoadAnimImage("graphics\snipe.facing_right_drop_mine.png",32,32,0,4)
Global snipe_facing_left_drop_dynamite:TImage = LoadAnimImage("graphics\snipe.facing_left_drop_dynamite.png",32,32,0,4)
Global snipe_facing_right_drop_dynamite:TImage = LoadAnimImage("graphics\snipe.facing_right_drop_dynamite.png",32,32,0,4)
Global snipe_trigger_dynamite_left:TImage = LoadAnimImage("graphics\snipe.trigger_dynamite_left.png",32,32,0,8)
Global snipe_trigger_dynamite_right:TImage = LoadAnimImage("graphics\snipe.trigger_dynamite_right.png",32,32,0,8)
Global snipe_jump_left:TImage = LoadImage("graphics\snipe.jump_left.png")
Global snipe_jump_right:TImage = LoadImage("graphics\snipe.jump_right.png")
Global snipe_die:TImage = LoadAnimImage("graphics\snipe.die.png",32,32,0,4)


Global gripe_stand_left:TImage = LoadAnimImage("graphics\gripe.stand_left.png",32,32,0,1)
Global gripe_stand_right:TImage = LoadAnimImage("graphics\gripe.stand_right.png",32,32,0,1)
Global gripe_run_left:TImage = LoadAnimImage("graphics\gripe.run_left.png",32,32,0,8)
Global gripe_run_right:TImage = LoadAnimImage("graphics\gripe.run_right.png",32,32,0,8)
Global gripe_skid_halt_left:TImage = LoadAnimImage("graphics\gripe.skid_halt_left.png",32,32,0,4)
Global gripe_skid_halt_right:TImage = LoadAnimImage("graphics\gripe.skid_halt_right.png",32,32,0,4)
Global gripe_turn_left_to_right:TImage = LoadAnimImage("graphics\gripe.turn_left_to_right.png",32,32,0,4)
Global gripe_turn_right_to_left:TImage = LoadAnimImage("graphics\gripe.turn_right_to_left.png",32,32,0,4)
Global gripe_blink_facing_left:TImage = LoadAnimImage("graphics\gripe.blink_facing_left.png",32,32,0,8)
Global gripe_blink_facing_right:TImage = LoadAnimImage("graphics\gripe.blink_facing_right.png",32,32,0,8)
Global gripe_facing_left_drop_bomb:TImage = LoadAnimImage("graphics\gripe.facing_left_drop_bomb.png",32,32,0,4)
Global gripe_facing_right_drop_bomb:TImage = LoadAnimImage("graphics\gripe.facing_right_drop_bomb.png",32,32,0,4)
Global gripe_facing_left_drop_mine:TImage = LoadAnimImage("graphics\gripe.facing_left_drop_mine.png",32,32,0,4)
Global gripe_facing_right_drop_mine:TImage = LoadAnimImage("graphics\gripe.facing_right_drop_mine.png",32,32,0,4)
Global gripe_facing_left_drop_dynamite:TImage = LoadAnimImage("graphics\gripe.facing_left_drop_dynamite.png",32,32,0,4)
Global gripe_facing_right_drop_dynamite:TImage = LoadAnimImage("graphics\gripe.facing_right_drop_dynamite.png",32,32,0,4)
Global gripe_trigger_dynamite_left:TImage = LoadAnimImage("graphics\gripe.trigger_dynamite_left.png",32,32,0,8)
Global gripe_trigger_dynamite_right:TImage = LoadAnimImage("graphics\gripe.trigger_dynamite_right.png",32,32,0,8)
Global gripe_jump_left:TImage = LoadImage("graphics\gripe.jump_left.png")
Global gripe_jump_right:TImage = LoadImage("graphics\gripe.jump_right.png")
Global gripe_die:TImage = LoadAnimImage("graphics\gripe.die.png",32,32,0,4)


Global spinning_coin_gold:TImage = LoadAnimImage("graphics\spinning_coin_gold.png",16,16,0,8)
Global spinning_coin_silver:TImage = LoadAnimImage("graphics\spinning_coin_silver.png",16,16,0,8)
Global coin_collide:TImage = LoadImage("graphics\coin_collide.png")
Global picked_up_16x16:TImage = LoadAnimImage("graphics\picked_up_16x16.png",16,16,0,12)


Global gem_10_points:TImage = LoadAnimImage("graphics\gem_10_points.png",16,16,0,4)
Global gem_20_points:TImage = LoadAnimImage("graphics\gem_20_points.png",16,16,0,4)
Global gem_40_points:TImage = LoadAnimImage("graphics\gem_40_points.png",16,16,0,4)
Global gem_80_points:TImage = LoadAnimImage("graphics\gem_80_points.png",16,16,0,4)
Global gem_100_points:TImage = LoadAnimImage("graphics\gem_100_points.png",16,16,0,4)
Global gem_150_points:TImage = LoadAnimImage("graphics\gem_150_points.png",16,16,0,4)
Global gem_collide:TImage = LoadImage("graphics\gem_collide.png")

Global invincibility_pickup:TImage = LoadAnimImage("graphics\invincibility_pickup.png",32,32,0,8)
Global invincibility_picked_up:TImage = LoadAnimImage("graphics\invincibility_picked_up.png",32,32,0,6)

Global wheelie_left:TImage = LoadAnimImage("graphics\wheelie_left.png",32,32,0,4)
Global wheelie_right:TImage = LoadAnimImage("graphics\wheelie_right.png",32,32,0,4)
Global wheelie_die_left:TImage = LoadImage("graphics\wheelie_die_left.png")
Global wheelie_die_right:TImage = LoadImage("graphics\wheelie_die_right.png")

Global grog_left:TImage = LoadAnimImage("graphics\grog.left.png",32,32,0,8)
Global grog_right:TImage = LoadAnimImage("graphics\grog.right.png",32,32,0,8)
Global grog_die_left:TImage = LoadImage("graphics\grog_die_left.png")
Global grog_die_right:TImage = LoadImage("graphics\grog_die_right.png")

Global slinky_all:TImage = LoadAnimImage("graphics\slinky.png",32,32,0,32)
Global slinky_fall:TImage = LoadAnimImage("graphics\slinky_fall.png",32,8,0,2)

Global smoke_16:TImage = LoadAnimImage("graphics\smoke_16x16.png",16,16,0,8)

Global bomb_floor:TImage = LoadAnimImage ("graphics\bomb_floor.png",16,16,0,4)
Global mine_floor:TImage = LoadAnimImage ("graphics\mine_floor.png",16,16,0,4)
Global dynamite_floor:TImage = LoadAnimImage ("graphics\dynamite_floor.png",16,16,0,4)
Global explosion_gfx:TImage = LoadAnimImage("graphics\explosion2.png",64,64,0,8)
Global icons:TImage = LoadAnimImage("graphics\icons.png",16,16,0,4)

Global exitpost_appear:TImage = LoadAnimImage("graphics\exitpost_appear.png",64,64,0,40)
Global exitpost_back:TImage = LoadAnimImage("graphics\exitpost_back.png",64,64,0,8)
Global exitpost_overlay:TImage = LoadAnimImage("graphics\exitpost_overlay.png",64,64,0,8)
Global exitpost_collision:TImage = LoadImage("graphics\exitpost_collision.png")
Global exitpost_disappear:TImage = LoadAnimImage("graphics\exitpost_disappear.png",64,64,0,32)

SetMaskColor 0,0,0
Global spritemasks :TImage = LoadAnimImage ("graphics\collmask.bmp", 32, 32, 0, 5)
SetMaskColor 255,0,255







'**** Set global variables ****
Global map[4,40,30]							'Store level map

Global fade_values#[]=[0.0,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95]


Global bomb_list:TList = CreateList()		'List for bombs
Global mine_list:TList = CreateList()		'List for mines
Global dynamite_list:TList = CreateList()	'List for dynamite
Global wheelie_list:TList = CreateList()	'List for wheelies
Global grog_list:TList = CreateList()		'List for grog
Global slinky_list:TList = CreateList()		'List for slinky
Global player_list:TList = CreateList()		'List for players
Global gems_list:TList = CreateList()		'List for gems
Global coins_list:TList = CreateList()		'List for coins
Global pickedup16_list:TList = CreateList()	'List for coins/gems pickup anim (16x16)
Global pickedup32_list:TList = CreateList()	'List for coins/gems pickup anim (32x32)
Global reward_list:TList = CreateList()		'List for rewards (points earned for killing enemies)
Global explosion_list:TList = CreateList()	'List for explosions
Global smoke_list:TList = CreateList()		'List for smoke clouds
Global pickup_list:TList = CreateList()		'List for pick ups
Global hiscoretable_list:TList = CreateList()'List for hi score entry
Global DestructTile_list:TList = CreateList()'List for destructable tiles





'**** START OF TYPE DECLARATIONS ****

Type TDestructTile
	Field x:Int,y:Int
	Field tile:Int,replacewith:Int
	Field replaceunder:Byte,replaceunderwith:Int
	

	Method AddToCollisionLayer()
		CollideImage tilesbitmap,x*16,y*16,tile,0,COL_DESTROY_TILE,Self
	End Method

	Function Spawn(tx:Int,ty:Int,t:Int)
		Local b:TDestructTile = New TDestructTile
		b.x=tx						'Set X coord
		b.y=ty						'Set Y coord
		b.tile=t
		Select t
			Case 51
				b.replacewith=0
				b.replaceunder=1
				b.replaceunderwith=102
				
			Default
				b.replacewith=0
				b.replaceunder=0			
		End Select
		ListAddLast DestructTile_list,b	
		'Return b						
	End Function

End Type

Global DestructTile:TDestructTile

Type Tbomb	'Type for bombs
	Field t,startframe,endframe,currframe,framecount,framecounttotal
	Field belongsto,fall,falltime
	Field image:TImage
	Field x#,y#,dx#,dy#,grip#
	Field kicked
	
	Method Update()				'Update bomb
		framecount=framecount-1		'Adjust framecounter (time till frame changes)
		If framecount=0 Then			'When framecounter=0
			framecount=framecounttotal		'Reset framecounter
			currframe=currframe+1			'Increase frame
			If currframe>endframe Then currframe=startframe
		End If
			
		If dx#<>0 Then
			x#=x#+dx#
			If dx#>0 Then dx#=dx#-grip#
			If dx#<0 Then dx#=dx#+grip#
			If CollideImage(image,x#,y#,currframe,2,0) Then
				x#=x#-dx#
				dx#=-dx#
			End If
 	 		If CollideImage(image,x#,y#,currframe,4,0) Then
				x#=x#-dx#
				dx#=-dx#
			End If
			If dx#>-.5 And dx#<.5 Then dx#=0
		End If
			
		If dy#<>0 Then
			y#=y#+dy#
			If dy#<0 Then
				dy#=dy#+.08
				If dy#>-.07 Then
					dy#=0
					kicked=False
				End If
			End If
		End If
			
		If kicked=False Then
			Local z=0
			fall=0
			For z=0 To 3
				If Not CollideImage(coin_collide,x#,y#,0,1,0) Or CollideImage(coin_collide,x#,y#-4,0,1,0) Then
					y#=y#+1
					fall=1
				End If
			Next
			If fall Then		'Bomb is falling ground
				falltime=falltime+1
			Else				'Bomb has hit/is on ground
				If falltime>4 Then PlaySound(bomb_land_snd)
				falltime=0
			EndIf					
		End If
		
		CollideImage image,x#,y#,currframe,0,16,Self
			
		t=t-1						'Countdown bomb timer (fuse)
		If CollideImage(image,x#,y#,0,32,0) Or t=0 Then	'Once timer=0 or caught by explosion
			Texplosion.Spawn(x,y-24,belongsto)
			ListRemove bomb_list,Self		'Remove bomb from list
			bomb_list.remove(Self)
			currframe=3					'Avoid trying to draw non-existent frame
		End If
		If y#>524 Then					'If bomb drop off screen
			ListRemove bomb_list,Self		'Remove bomb from list
			bomb_list.remove(Self)
			currframe=3					'Avoid trying to draw non-existent frame
		End If
	End Method
	
	Method Draw()					'Draw bomb
		SetBlend (MASKBLEND)			'Set drawmode to mask
		If t>0 Then
			If t<50 And (currframe=0 Or currframe=2) Then SetBlend (LIGHTBLEND)
			Select currframe
					Case 0
						SetScale(1,1)
					Case 1,3
						SetScale(1.125,1.125)
					Case 2
						SetScale(1.25,1.25)
			End Select
		End If
		DrawImage image,x#,y#,currframe	'draw image to screen
		'SetAlpha(1)
		SetScale(1,1)
	End Method
	
	Function Spawn:Tbomb(px#,py#,pdx#,g#,time,sframe,eframe,acount,bel) 'Create new bomb
		Local b:Tbomb = New Tbomb		'Create new instance of bomb
		b.x#=px#						'Set X coord
		b.y#=py#						'Set Y coord
		b.dx#=pdx#
		b.grip#=g#/4
		b.t=time						'Set time (fuse length)
		b.belongsto=bel				'Record which player placed bomb
		b.currframe=sframe			'Current anim frame
		b.startframe=sframe			'Start anim frame
		b.endframe=eframe				'End anim frame
		b.framecount=acount			'Timer for animation speed
		b.framecounttotal=acount		'Total for animation speed timer
		b.image=bomb_floor			'Image gfx
		b.fall=0						'Is bomb falling
		b.kicked=False				'Has bomb been kicked
		b.falltime=0					'Time bomb spends falling (for sfx)
		ListAddLast bomb_list,b		'Add new bomb to bomb list
		Return b						'Return instance of bomb
	End Function
	
End Type

Global bomb:Tbomb						'Global bomb=instance of TBomb

Type Tmine	'Type for mine
	Field startframe,endframe,currframe,framecount,framecounttotal
	Field belongsto,time
	Field image:TImage
	Field x,y
	
	Method Update()				'Update mine
		framecount=framecount-1		'Adjust framecounter (time till frame changes)
		If framecount=0 Then			'When framecounter=0
			framecount=framecounttotal		'Reset framecounter
			If time=0 Then currframe=currframe+1			'Increase frame
			If currframe>endframe Then currframe=startframe
		End If
		If time>0 Then
			time=time-1
			If time=0 Then PlaySound (mine_arm_snd)
		End If
		If CollideImage(image,x,y,0,32,0)  Then	'Caught by explosion
			Texplosion.Spawn(x,y-24,belongsto)
			ListRemove mine_list,Self		'Remove mine from list
			mine_list.remove(Self)
			currframe=3					'Avoid trying to draw non-existent frame
		Else
			CollideImage image,x,y,currframe,0,1024,Self
		End If
	End Method

	
	Method Draw()					'Draw mine
		SetBlend (MASKBLEND)			'Set drawmode to mask
		'If CollideImage(image,x,y,0,32,0) Then t=1
		DrawImage image,x,y,currframe	'draw image to screen
	End Method
	
	Function Spawn:Tmine(mx,my,sframe,eframe,acount,bel) 'Create new mine
		Local m:Tmine = New Tmine		'Create new instance of mine
		m.x=mx						'Set X coord
		m.y=my						'Set Y coord
		m.belongsto=bel				'Record which player placed mine
		m.currframe=sframe			'Current anim frame
		m.startframe=sframe			'Start anim frame
		m.endframe=eframe				'End anim frame
		m.framecount=acount			'Timer for animation speed
		m.framecounttotal=acount		'Total for animation speed timer
		m.image=mine_floor			'Image gfx
		m.time=40
		ListAddLast mine_list,m		'Add new mine to mine list
		Return m						'Return instance of mine
	End Function
	
End Type

Global mine:Tmine						'Global mine=instance of Tmine

Type Tdynamite	'Type for dynamite
	Field startframe,endframe,currframe,framecount,framecounttotal
	Field belongsto
	Field image:TImage
	Field x,y
	
	Method Update()				'Update bomb
		framecount=framecount-1		'Adjust framecounter (time till frame changes)
		If framecount=0 Then			'When framecounter=0
			framecount=framecounttotal		'Reset framecounter
			currframe=currframe+1			'Increase frame
			If currframe>endframe Then currframe=startframe
		End If
		
		If CollideImage(image,x,y,0,32,0)  Then	'Caught by explosion
			Texplosion.Spawn(x,y-24,belongsto)
			'Cycle though player until we find the one's dynamite has detonated
				For Local tempp:Tplayer = EachIn player_list
					If tempp.id=belongsto Then
						tempp.tnt_laid=0 'clear dynamite laid flag for player
					End If
				Next

			ListRemove dynamite_list,Self		'Remove dynamite from list
			dynamite_list.remove(Self)
			currframe=3					'Avoid trying to draw non-existent frame
		End If
		
	End Method

	
	Method Draw()					'Draw mine
		SetBlend (MASKBLEND)			'Set drawmode to mask
		'If CollideImage(image,x,y,0,32,0) Then t=1
		DrawImage image,x,y,currframe	'draw image to screen
	End Method
	
	Function Spawn:Tdynamite(dx,dy,sframe,eframe,acount,bel) 'Create new dynamite
		Local d:Tdynamite = New Tdynamite		'Create new instance of dynamite
		d.x=dx						'Set X coord
		d.y=dy						'Set Y coord
		d.belongsto=bel				'Record which player placed dynamite
		d.currframe=sframe			'Current anim frame
		d.startframe=sframe			'Start anim frame
		d.endframe=eframe				'End anim frame
		d.framecount=acount			'Timer for animation speed
		d.framecounttotal=acount		'Total for animation speed timer
		d.image=dynamite_floor			'Image gfx
		PlaySound (mine_arm_snd)
		ListAddLast dynamite_list,d		'Add new dynamite to dynamite list
		Return d						'Return instance of dynamite
	End Function
	
End Type

Global dynamite:Tdynamite						'Global dynamite=instance of Tdynamite

Type Texplosion
	Field currframe,framecount,framecounttotal
	Field y,x,belongsto
	Field image:TImage
	
	
	Method Update()
		framecount=framecount-1			'Adjust framecounter
		If framecount=0 Then				'If framecounter=0 then
			framecount=framecounttotal		'Reset framecounter
			currframe=currframe+1			'Increase frame
			If currframe=4 Then
				Tsmoke.Spawn(x,y+4,0,0)
				Tsmoke.Spawn(x+12,y,1,.125)
				Tsmoke.Spawn(x+22,y-4,2,.25)
				Tsmoke.Spawn(x+12,y-8,3,.125)
				TSmoke.Spawn(x,y-12,4,0)
				Tsmoke.Spawn(x-12,y-8,5,-.125)
				Tsmoke.Spawn(x-22,y-4,6,-.25)
				Tsmoke.Spawn(x-12,y,7,-.125)
			End If
			If currframe>7 Then			'If end of frames
				ListRemove explosion_list,Self		'Remove explosion from list
				explosion_list.remove(Self)
				currframe=7					'Avoid trying to draw non-existent frame
			Else
				If currframe<4 Then
					Local cc:Object[]=CollideImage(image,x,y,currframe,COL_DESTROY_TILE,0)
					If cc Then
						For Local temptile:TDestructTile = EachIn DestructTile_list
							If temptile=cc[0] Then
								map[3,temptile.x,temptile.y]=0
								map[1,temptile.x,temptile.y]=temptile.replacewith
								If temptile.replaceunder Then map[1,temptile.x,temptile.y+1]=temptile.replaceunderwith
								ListRemove DestructTile_list,temptile
							End If
						Next
					End If
					CollideImage image,x,y,currframe,0,COL_EXPLOSION,Self
				End If
			End If
		End If

		
	End Method
	
	Method Draw()
		SetBlend (MASKBLEND)			'Set drawmode to mask
		DrawImage image,x,y,currframe	'draw image to screen
	End Method
	
	Function Spawn:Texplosion(ex,ey,b)
		Local e:Texplosion = New Texplosion
		e.x=ex
		e.y=ey
		e.belongsto=b
		e.currframe=0
		e.framecount=4
		e.framecounttotal=4
		e.image=explosion_gfx				'Set  image to explosion gfx
		ListAddLast explosion_list,e
		PlaySound explosion_snd
		Return e
	End Function
End Type

Global explosion:Texplosion


Type Tsmoke
	Field currframe,framecount,framecounttotal,time
	Field alpha#,y#,x#,dx#
	
	
	Method Update()

		
		
		y#=y#-.5
		alpha#=alpha#-.015
		x#=x#+dx#
		
		framecount=framecount+1
		If framecount>framecounttotal Then
			currframe=currframe+1
			If currframe>7 Then currframe=0
			framecount=0
		End If
		
		If y#<-16 Or alpha#<=0 Then
			ListRemove smoke_list,Self
			smoke_list.remove(Self)
		End If
		
	End Method
	
	Method Draw()
		SetBlend (ALPHABLEND)
		SetAlpha (alpha#)
		DrawImage smoke_16,x#,y#,currframe
		SetBlend (MASKBLEND)
		SetAlpha(1)
	End Method
	
	Function Spawn:Tsmoke(sx#,sy#,f,sdx#)
		Local s:Tsmoke = New Tsmoke
		s.x#=sx#
		s.y#=sy#
		s.dx#=sdx#
		s.alpha#=.8
		s.currframe=f
		s.framecount=0
		s.framecounttotal=8
		s.time=32
		ListAddLast smoke_list,s
		Return s
	End Function
End Type

Global smoke:Tsmoke

Type Twheelie
	Field x,y,dx,fall,value,hitbombtimer,hitwalltimer
	Field startframe,endframe,currframe,framecount,framecounttotal
	Field explode
	Field dietime,dieflash
	Field isjumping,jumplevel,jumptimer,jumpdir,tryjumpagain
	Field fatness:Float
	Field image:TImage
	
	Method Update()
		'Check for collision with explosion
		If hitbombtimer>0 Then hitbombtimer:-1
		If hitwalltimer>0 Then hitwalltimer:-1
		If explode=False Then

			If dx<0 Then image=wheelie_left Else image=wheelie_right
			
			Local temp:Object[]
			temp=CollideImage(image,x,y,currframe,32,0)
			If temp Then
				'Cycle through bombs until we find the one that matches collision
				Local tempid:Int	'Used to hold the id of which player placed bomb
				For Local tempexp:Texplosion = EachIn explosion_list
					If tempexp=temp[0] Then
						tempid=tempexp.belongsto
					End If
				Next
				'Cycle though player until we find the one with the ID that placed the bomb
				For Local tempp:Tplayer = EachIn player_list
					If tempp.id=tempid Then
						tempp.score=tempp.score+value 'Increase correct players score
					End If
				Next
				explode=True
				currframe=0
				dietime=100
				dieflash=0
				Treward.Spawn(x-12,y-16,value)
			End If
			
			'Collision detection for mines
			temp=CollideImage(image,x,y,currframe,1024,0)
			If temp Then
				For Local tempmine:Tmine = EachIn mine_list
					If tempmine=temp[0] Then
						If tempmine.time=0 Then
							Texplosion.Spawn(tempmine.x,tempmine.y-24,tempmine.belongsto)							
							ListRemove mine_list,tempmine
						End If
					End If
				Next
			End If

			
		End If
		If explode=False Then
			Local z=0
			
			If isjumping=0 Then
				For z=0 To 3
					fall=0
					If Not CollideImage(spritemasks,x,y,0,COL_FOREG,0) Then
						y:+1
						fall=1
					Else
						If CollideImage(spritemasks,x,y-5,4,COL_FOREG,0) Then
							fall=1
							y:+1
						End If
					End If
				Next			
			End If

			'Check for collision with unexploded bomb
			If CollideImage(image,x,y,currframe,COL_BOMB,0) And hitbombtimer=0 Then
				If fall=1 Then
					y=y-1
					fall=0
				Else
					dx=-dx
					hitbombtimer=32
				EndIf
			End If

			If y>=496 Then y=-16
		
			If fall=0 Then
				x=x+dx
				If x>=642 Then x=-4
				If x<=-6 Then x=640
			End If
			
			If CollideImage(image,x,y,currframe,COL_LEFTH,0) And hitwalltimer=0 Then
				x=x-dx
				dx=-dx
				hitwalltimer=32
			End If
 	 		If CollideImage(image,x,y,currframe,COL_RIGHTH,0) And hitwalltimer=0 Then
				x=x-dx
				dx=-dx
				hitwalltimer=32
			End If
			
			If Rand(0,300)=5 Or tryjumpagain=1 And fall=0 And isjumping=0 Then		'Will enemy jump up to next platform
				Local xgrid,ygrid
				xgrid=((x-16) Shr 4) + 1
				If xgrid>=0 And xgrid<40 Then
					ygrid=((y-16) Shr 4) +1
					Local isplatform=0
					Local topofcheck=ygrid-5
					If topofcheck<0 Then topofcheck=0
					Local botofcheck=ygrid-2
					If botofcheck>29 Then botofcheck=29
					If botofcheck>topofcheck Then
						For Local z:Int=topofcheck To botofcheck
							If map[3,xgrid,z]=8 Then isplatform=z
						Next
						If isplatform>0 Then
							isjumping=1
							jumplevel=(isplatform-1)*16
							jumptimer=24
							fatness=0
							dx=0
						End If
					End If
				End If
			End If
			
			If fall=0 Then tryjumpagain=0
		
		
			framecount=framecount-1
			If framecount=0 Then
				framecount=framecounttotal
				currframe=currframe+1
				If currframe>endframe Then currframe=startframe
			End If
			
			
			If isjumping=2 Then
				y:-8
				'If y<=jumplevel-8 Then isjumping=0
				fatness=.5
				If y<=jumplevel-8 Then
					isjumping=0
					dx=Rand(0,1)
					If dx=0 Then dx=-1
					tryjumpagain=Rand(0,1)
				End If
			End If
			
			
			If isjumping=1 Then
				currframe=0
				jumptimer:-1
				If (jumptimer Shr 3) Shl 3 = jumptimer Then
					'jumpdir=1-jumpdir
					fatness:+.32
				End If
				If jumpdir Then image=wheelie_left Else image=wheelie_right
				If jumptimer=0 Then isjumping=2
			End If
	
		Else
			If dx<0 Then image=wheelie_die_left Else image=wheelie_die_right
			
			dietime=dietime-1
			If dietime<40 Then dieflash=1-dieflash
			If dietime=0 Then
				ListRemove wheelie_list,Self
				wheelie_list.remove(Self)
			End If
		End If
	End Method
	
	Method Appear(s#)
		SetBlend (MASKBLEND)
		'SetScale (1,s#)
		'DrawImage enemy1,x,y+16-(16*s#),currframe
		SetScale (4-(3*s#),s#)
		DrawImage image,x,y,currframe
		SetScale (1,1)
	End Method
	
	Method Draw()
		If dieflash Then
			SetBlend(LIGHTBLEND)
			If dietime<20 Then SetAlpha(fade_values#[dietime])
		Else
			SetBlend (MASKBLEND)
		End If
		If isjumping=1 Then
			'SetScale(1+fatness,1-(fatness/2))
			SetScale(1,1-(fatness/2))
			'DrawImage image,x-fatness*16,y+fatness*16,currframe
			DrawImage image,x,y+fatness*8,currframe
		End If
		If isjumping=2 Then
			SetScale(0.5,2)
			'DrawImage image,x-fatness*16,y+fatness*16,currframe
			DrawImage image,x,y-16,currframe
		End If
		If isjumping=0 Then
			DrawImage image,x,y,currframe
		End If
		If explode=False Then CollideImage image,x,y,currframe,0,8,Null
		SetAlpha(1)
		SetScale(1,1)
	End Method
	
	Function Spawn:Twheelie(px,py,dir)
		Local e:Twheelie = New Twheelie
		e.x=px					'X position
		e.y=py					'Y position
		e.dx=dir					'X direction
		e.startframe=0		'Start anim frame
		e.endframe=3			'End anim frame
		e.framecount=8		'Counter for animation delay
		e.value=100
		e.currframe=e.startframe		'Current anim frame
		e.framecounttotal=e.framecount	'Max for animation delay counter
		e.explode=False				'Is enemy currently exploding/dying
		e.image=wheelie_left
		e.dieflash=0
		e.isjumping=0
		ListAddLast wheelie_list,e
		Return e
	End Function
	
End Type

Global wheelie:Twheelie

Type Tgrog
	Field x,y,dx,fall,value,hitbombtimer,hitwalltimer
	Field startframe,endframe,currframe,framecount,framecounttotal
	Field explode
	Field dietime,dieflash
	Field isjumping,jumplevel,jumptimer,jumpdir,jumpface,tryjumpagain
	Field fatness:Float
	Field image:TImage
	
	Method Update()
		'Check for collision with explosion
		If hitbombtimer>0 Then hitbombtimer:-1
		If hitwalltimer>0 Then hitwalltimer:-1
		If explode=False Then

			If dx<0 Then image=grog_left Else image=grog_right
			
			Local temp:Object[]
			temp=CollideImage(image,x,y,currframe,32,0)
			If temp Then
				'Cycle through bombs until we find the one that matches collision
				Local tempid:Int	'Used to hold the id of which player placed bomb
				For Local tempexp:Texplosion = EachIn explosion_list
					If tempexp=temp[0] Then
						tempid=tempexp.belongsto
					End If
				Next
				'Cycle though player until we find the one with the ID that placed the bomb
				For Local tempp:Tplayer = EachIn player_list
					If tempp.id=tempid Then
						tempp.score=tempp.score+value 'Increase correct players score
					End If
				Next
				explode=True
				currframe=0
				dietime=100
				dieflash=0
				Treward.Spawn(x-12,y-16,value)
			End If
			
			'Collision detection for mines
			temp=CollideImage(image,x,y,currframe,1024,0)
			If temp Then
				For Local tempmine:Tmine = EachIn mine_list
					If tempmine=temp[0] Then
						If tempmine.time=0 Then
							Texplosion.Spawn(tempmine.x,tempmine.y-24,tempmine.belongsto)							
							ListRemove mine_list,tempmine
						End If
					End If
				Next
			End If

			
		End If
		If explode=False Then
			
			Local xdrop:Int=(((x - 16) + dx*16) Shr 4) + 1  'Check for playform beneath feet
			Local ydrop:Int=(y Shr 4) +1
			'DrawRect xdrop*16,ydrop*16,16,16
			If xdrop>=0 And xdrop<40 And ydrop>=0 And ydrop<30 Then
				If map[3,xdrop,ydrop]<>8 Then dx=-dx
			End If
		
			''Check for collision with unexploded bomb
			'If CollideImage(image,x,y,currframe,COL_BOMB,0) And hitbombtimer=0 Then
			'	If fall=1 Then
			'		y=y-1
			'		fall=0
			'	Else
			'		dx=-dx
			'		hitbombtimer=32
			'	EndIf
			'End If

			If y>=496 Then y=-16
		
			If fall=0 Then
				x=x+dx
				If x>=642 Then x=-4
				If x<=-6 Then x=640
			End If
			
			If CollideImage(image,x,y,currframe,COL_LEFTH,0) And hitwalltimer=0 Then
				x=x-dx
				dx=-dx
				hitwalltimer=32
			End If
 	 		If CollideImage(image,x,y,currframe,COL_RIGHTH,0) And hitwalltimer=0 Then
				x=x-dx
				dx=-dx
				hitwalltimer=32
			End If
			
			Local rtemp:Int=Rand(0,300)
			If (rtemp=5 Or tryjumpagain=1) And fall=0 And isjumping=0 Then		'Will enemy jump up to next platform
				Local xgrid,ygrid
				xgrid=((x-16) Shr 4) + 1
				If xgrid>=0 And xgrid<40 Then
					ygrid=((y-16) Shr 4) +1
					Local isplatform=0
					Local topofcheck=ygrid-8
					If topofcheck<0 Then topofcheck=0
					Local botofcheck=ygrid-2
					If botofcheck>29 Then botofcheck=29
					If botofcheck>topofcheck Then
						For Local z:Int=topofcheck To botofcheck
							If map[3,xgrid,z]=8 Then
								isplatform=z
								Local morethanone:Int=0
								If xgrid>0 Then
									If map[3,xgrid-1,z]=8 Then morethanone:+1
								End If
								If xgrid>1 Then
									If map[3,xgrid-2,z]=8 Then morethanone:+1
								End If
								If xgrid<39 Then
									If map[3,xgrid+1,z]=8 Then morethanone:+1
								End If
								If xgrid<38 Then
									If map[3,xgrid+2,z]=8 Then morethanone:+1
								End If
								If morethanone<2 Then isplatform=0
							End If
						Next
						If isplatform>0 Then
							isjumping=1
							jumplevel=(isplatform-1)*16
							jumptimer=96
							fatness=0
							dx=0
							jumpdir=-8
						End If
					End If
				End If
			End If
			
			If rtemp=4 And tryjumpagain=0 And fall=0 And isjumping=0 Then		'Will enemy jump up to next platform
				Local xgrid,ygrid
				xgrid=((x-16) Shr 4) + 1
				If xgrid>=0 And xgrid<40 Then
					ygrid=((y+16) Shr 4) +1
					Local isplatform=0
					Local topofcheck=ygrid
					If topofcheck<0 Then topofcheck=0
					Local botofcheck=29
					If botofcheck>topofcheck Then
						For Local z:Int=botofcheck To topofcheck Step -1
							If map[3,xgrid,z]=8 Then
								isplatform=z
								Local morethanone:Int=0
								If xgrid>0 Then
									If map[3,xgrid-1,z]=8 Then morethanone:+1
								End If
								If xgrid>1 Then
									If map[3,xgrid-2,z]=8 Then morethanone:+1
								End If
								If xgrid<39 Then
									If map[3,xgrid+1,z]=8 Then morethanone:+1
								End If
								If xgrid<38 Then
									If map[3,xgrid+2,z]=8 Then morethanone:+1
								End If
								If morethanone<2 Then isplatform=0
							End If
						Next
						If isplatform>0 Then
							isjumping=1
							jumplevel=(isplatform-1)*16
							jumptimer=96
							fatness=0
							dx=0
							jumpdir=8
						End If
					End If
				End If
			End If
			
			If fall=0 Then tryjumpagain=0
		
		
			framecount=framecount-1
			If framecount=0 Then
				framecount=framecounttotal
				currframe=currframe+1
				If currframe>endframe Then currframe=startframe
			End If
			
			
			If isjumping=2 Then
				y:+jumpdir
				'If y<=jumplevel-8 Then isjumping=0
				If (jumpdir<0 And y<=jumplevel) Or (jumpdir>0 And y>=jumplevel) Then
					isjumping=0
					y=jumplevel+2
					dx=Rand(0,1)
					If dx=0 Then dx=-1
					If jumpdir<0 Then tryjumpagain=Rand(0,1) Else tryjumpagain=0
				End If
			End If
			
			
			If isjumping=1 Then
				currframe=0
				jumptimer:-1
				If (jumptimer Shr 3) Shl 3 = jumptimer Then
					jumpface=1-jumpface
					fatness:+.08
				End If
				If jumpface Then image=grog_left Else image=grog_right
				If jumptimer=0 Then isjumping=2
			End If
	
		Else
			If dx<0 Then image=grog_die_left Else image=grog_die_right
			
			dietime=dietime-1
			If dietime<40 Then dieflash=1-dieflash
			If dietime=0 Then
				ListRemove grog_list,Self
				grog_list.remove(Self)
			End If
		End If
	End Method
	
	Method Appear(s#)
		SetBlend (MASKBLEND)
		'SetScale (1,s#)
		'DrawImage enemy1,x,y+16-(16*s#),currframe
		SetScale (4-(3*s#),s#)
		DrawImage image,x,y,currframe
		SetScale (1,1)
	End Method
	
	Method Draw()
		If dieflash Then
			SetBlend(LIGHTBLEND)
			If dietime<20 Then SetAlpha(fade_values#[dietime])
		Else
			SetBlend (MASKBLEND)
		End If
		If isjumping=1 Then
			SetScale(1+fatness,1-(fatness/2))
			'DrawImage image,x-fatness*16,y+fatness*16,currframe
			DrawImage image,x,y+fatness*8,currframe
		Else
			DrawImage image,x,y,currframe
		End If
		If explode=False Then CollideImage image,x,y,currframe,0,8,Null
		SetAlpha(1)
		SetScale(1,1)
	End Method
	
	Function Spawn:Tgrog(px,py,dir)
		Local e:Tgrog = New Tgrog
		e.x=px					'X position
		e.y=py					'Y position
		e.dx=dir					'X direction
		e.startframe=0		'Start anim frame
		e.endframe=7			'End anim frame
		e.framecount=8		'Counter for animation delay
		e.value=150
		e.currframe=e.startframe		'Current anim frame
		e.framecounttotal=e.framecount	'Max for animation delay counter
		e.explode=False				'Is enemy currently exploding/dying
		e.image=grog_left
		e.dieflash=0
		e.isjumping=0
		ListAddLast grog_list,e
		Return e
	End Function
	
End Type

Global grog:Tgrog


Type Tslinky
	Field x,y,dx,fall,value,hitbombtimer,hitwalltimer,oldy
	Field startframe,endframe,currframe,framecount,framecounttotal,fallframe
	Field explode
	Field dietime,dieflash
	Field isjumping,jumplevel,jumpdir,jumpsegs
	Field fatness:Float
	Field image:TImage
	
	Method Update()
		'Check for collision with explosion
		If hitbombtimer>0 Then hitbombtimer:-1
		If hitwalltimer>0 Then hitwalltimer:-1
		If explode=False Then

			image=slinky_all
			
			Local temp:Object[]
			temp=CollideImage(image,x,y,currframe,32,0)
			If temp Then
				'Cycle through bombs until we find the one that matches collision
				Local tempid:Int	'Used to hold the id of which player placed bomb
				For Local tempexp:Texplosion = EachIn explosion_list
					If tempexp=temp[0] Then
						tempid=tempexp.belongsto
					End If
				Next
				'Cycle though player until we find the one with the ID that placed the bomb
				For Local tempp:Tplayer = EachIn player_list
					If tempp.id=tempid Then
						tempp.score=tempp.score+value 'Increase correct players score
					End If
				Next
				explode=True
				currframe=0
				dietime=100
				dieflash=0
				Treward.Spawn(x-12,y-16,value)
			End If
			
			'Collision detection for mines
			temp=CollideImage(image,x,y,currframe,1024,0)
			If temp Then
				For Local tempmine:Tmine = EachIn mine_list
					If tempmine=temp[0] Then
						If tempmine.time=0 Then
							Texplosion.Spawn(tempmine.x,tempmine.y-24,tempmine.belongsto)							
							ListRemove mine_list,tempmine
						End If
					End If
				Next
			End If

			
		End If
		If explode=False Then
		
	
			If isjumping=0 And currframe=15 Then
				'Local xdrop:Int=(((x - 20)+(16*dx) ) Shr 4) +1  'Check for platform beneath feet
				Local xdrop:Int=(x Shr 4)  'Check for platform beneath feet
				If dx=-1 Then xdrop:+dx

				Local ydrop:Int=(y Shr 4) +1
				'DrawRect xdrop*16,ydrop*16,16,16
				If xdrop>=0 And xdrop<40 And ydrop>=0 And ydrop<30 Then
					If map[3,xdrop,ydrop]<>8 Then				'If not then fall
						Local xgrid,ygrid
						xgrid=((x-16) Shr 4) + 1
						xgrid=xdrop
						If xgrid>=0 And xgrid<40 Then
							ygrid=((y+16) Shr 4) +1
							Local isplatform=0
							Local topofcheck=ygrid
							If topofcheck<0 Then topofcheck=0
							Local botofcheck=29
							If botofcheck>=topofcheck Then
								For Local z:Int=botofcheck To topofcheck Step -1
									If map[3,xgrid,z]=8 Then isplatform=z
								Next
							End If
							If topofcheck>29 Then topofcheck=29
							If isplatform=0 Then
								For Local z:Int=topofcheck To 1 Step -1
									If map[3,xgrid,z]=8 Then isplatform=z
								Next							
							End If
							If isplatform>0 Then
								isjumping=1
								jumplevel=(isplatform-1)*16
								'dx=0
								jumpdir=8
								jumpsegs=0
								oldy=y
							End If
						End If
					End If
				End If
			End If
			
			
			If CollideImage(image,x,y,currframe,COL_LEFTH,0) And hitwalltimer=0 Then
				'x=x-dx
				dx=-dx
				'hitwalltimer=32
			End If
 	 		If CollideImage(image,x,y,currframe,COL_RIGHTH,0) And hitwalltimer=0 Then
				'x=x-dx
				dx=-dx
				'hitwalltimer=32
			End If
			
			'If isjumping=0 Then
				framecount=framecount-1
				If framecount=0 Then
					framecount=framecounttotal
					currframe=currframe+dx
					If currframe>endframe Then
						If isjumping=0 Then
							currframe=startframe
							x=x+16
							If x>656 Then x=-16
							Local xdrop:Int=(x Shr 4)  'Check for platform beneath feet
							Local ydrop:Int=(y Shr 4)+1
							If xdrop>0 And xdrop<40 Then
								If map[3,xdrop,ydrop]=16 Then
									dx=-dx
									currframe=endframe
									x:-16
								End If
							End If							
						Else
							currframe=endframe
						End If							
					End If
					If currframe<startframe Then
						If isjumping=0 Then
							currframe=endframe
							x=x-16
							If x<-16 Then x=656
					 	 	Local xdrop:Int=(x Shr 4)-1  'Check for platform beneath feet
							Local ydrop:Int=(y Shr 4)+1
							If xdrop>0 And xdrop<40 Then
								If map[3,xdrop,ydrop]=2 Then
									dx=-dx
									currframe=startframe
									x:+16
								End If
							End If							
						Else
							currframe=startframe
						End If
					End If
				End If
			'End If
			
			
			If isjumping=1 Then
				y:+jumpdir
				If y>=484 Then y=4
				jumpsegs:+1
				If jumpsegs>15 Then
					oldy:+jumpdir
					If oldy>=484 Then oldy=4
					jumpsegs:-1
				End If
				image=slinky_fall
				If dx<0 Then fallframe=0 Else fallframe=1
				'If y<=jumplevel-8 Then isjumping=0
				'If (jumpdir<0 And y<=jumplevel) Or (jumpdir>0 And y>=jumplevel) Then
				If ((y Shr 3) Shl 3) = jumplevel Then
					isjumping=2
					y=jumplevel+2
				End If
			End If
			
			If isjumping=2 Then
				oldy:+jumpdir
				If oldy>=484 Then oldy=4
				jumpsegs:-1
				image=slinky_fall
				If dx<0 Then fallframe=0 Else fallframe=1
				'If y<=jumplevel-8 Then isjumping=0
				If ((oldy Shr 3) Shl 3)=((y Shr 3) Shl 3)-8 Then
					isjumping=0
					'dx=Rand(0,1)
					'If dx=0 Then dx=-1
					image=slinky_all
					If dx<0 Then
						currframe=29
						x:-16
					Else
						currframe=2
						x:+16
					End If
				End If
			End If
			
		Else
			If dx<0 Then image=wheelie_die_left Else image=wheelie_die_right
			
			dietime=dietime-1
			If dietime<40 Then dieflash=1-dieflash
			If dietime=0 Then
				ListRemove slinky_list,Self
				slinky_list.remove(Self)
			End If
		End If
	End Method
	
	Method Appear(s#)
		SetBlend (MASKBLEND)
		'SetScale (1,s#)
		'DrawImage enemy1,x,y+16-(16*s#),currframe
		SetScale (4-(3*s#),s#)
		DrawImage image,x,y,currframe
		SetScale (1,1)
	End Method
	
	Method Draw()
		If dieflash Then
			SetBlend(LIGHTBLEND)
			If dietime<20 Then SetAlpha(fade_values#[dietime])
		Else
			SetBlend (MASKBLEND)
		End If
		If isjumping>0 Then
			If isjumping=1 Then
				DrawImage slinky_all,x,oldy,currframe
				If explode=False Then CollideImage slinky_all,x,oldy,currframe,0,8,Null
			End If
			Local tempy:Int=0
			For Local z:Int=0 To jumpsegs-1
				tempy=oldy+20+z*8
				If tempy>=484 Then tempy:-484
				DrawImage image,x,tempy,fallframe
				If explode=False Then CollideImage image,x,tempy,fallframe,0,8,Null
			Next
		Else
			DrawImage image,x,y,currframe
			If explode=False Then CollideImage image,x,y,currframe,0,8,Null
		End If
		SetAlpha(1)
		SetScale(1,1)
	End Method
	
	Function Spawn:Tslinky(px,py,dir)
		Local e:Tslinky = New Tslinky
		e.x=SnaptoGrid16(px)		'X position
		e.y=SnaptoGrid16(py)+2		'Y position
		e.dx=dir					'X direction
		e.startframe=0			'Start anim frame
		e.endframe=31			'End anim frame
		e.framecount=1		'Counter for animation delay
		e.value=200
		If dir=1 Then e.currframe=e.startframe Else e.currframe=e.endframe		'Current anim frame
		e.framecounttotal=e.framecount	'Max for animation delay counter
		e.explode=False				'Is enemy currently exploding/dying
		e.image=slinky_all
		e.dieflash=0
		e.isjumping=0
		ListAddLast slinky_list,e
		Return e
	End Function
	
End Type

Global slinky:Tslinky


Type Tpickedup16
	Field x,y,currframe,framecount
	
	Method Update()

		framecount:+1
		If framecount>2 Then
			framecount=0
			currframe:+1
			If currframe>11 Then
				currframe=11
				ListRemove pickedup16_list,Self
				pickedup16_list.remove(Self)
			End If
		End If
		
	End Method
	
	Method Draw()
		DrawImage picked_up_16x16,x,y,currframe
	End Method
	
	Function Spawn:Tpickedup16(px,py)
		Local p:Tpickedup16 = New Tpickedup16
		p.x=px
		p.y=py
		p.currframe=0		
		p.framecount=0
		ListAddLast pickedup16_list,p
		Return p
	End Function
End Type

Global pickedup16:Tpickedup16


Type Tpickedup32
	Field x,y,currframe,framecount
	
	Method Update()

		framecount:+1
		If framecount>5 Then
			framecount=0
			currframe:+1
			If currframe>5 Then
				currframe=5
				ListRemove pickedup32_list,Self
				pickedup32_list.remove(Self)
			End If
		End If
		
	End Method
	
	Method Draw()
		DrawImage invincibility_picked_up,x,y,currframe
	End Method
	
	Function Spawn:Tpickedup32(px,py)
		Local p:Tpickedup32 = New Tpickedup32
		p.x=px
		p.y=py
		p.currframe=0		
		p.framecount=0
		ListAddLast pickedup32_list,p
		Return p
	End Function
End Type

Global pickedup32:Tpickedup32


Type Treward
	Field x,y#,value
	Field alpha#
	
	Method Update()

		alpha#=alpha#-.01
		
		y#=y#-.5
		
		If y#<-8 Or alpha<=0 Then
			ListRemove reward_list,Self
			reward_list.remove(Self)
		End If
		
	End Method
	
	Method Draw()
		SetBlend (ALPHABLEND)
		SetAlpha (alpha#)
		SmallText(value,x,y#)
		SetBlend (MASKBLEND)
		SetAlpha(1)
	End Method
	
	Function Spawn:Treward(rx,ry#,v)
		Local r:Treward = New Treward
		r.x=rx
		r.y#=ry#
		r.value=v
		r.alpha#=1
		ListAddLast reward_list,r
		Return r
	End Function
End Type

Global reward:Treward


Type Tpickup
	Field x,y,t,fall,falltime,existfor
	Field currframe,framecount,framecounttotal
	Field image:TImage,value
	
	Method Update()
		If framecount=0 Then
			currframe=currframe+1
			If currframe>7 Then currframe=0
		End If
		framecount=framecount+1
		If framecount>=framecounttotal Then framecount=0
		
		If fall=1 Then
			If falltime=0 Then
				If CollideImage(coin_collide,x,y+2,0,1,0) Then fall=0
			Else
				falltime=falltime-1
				If falltime=0 And CollideImage(coin_collide,x,y,0,1,0) Then falltime=1
			End If
		End If
			
		
		If fall=1 Then y=y+4
		
		existfor=existfor-1
		
		If y>480 Or existfor<1 Then
			ListRemove pickup_list,Self
			pickup_list.remove(Self)
		Else
			CollideImage image,x,y,currframe,0,2048,Self
		End If
		
	End Method
	
	Method Draw()
		SetBlend (MASKBLEND)
		DrawImage image,x,y,currframe
	End Method
	
	Function Spawn:Tpickup(px,py,acount,t,ft)
		Local p:Tpickup = New Tpickup
		p.x=px
		p.y=py
		p.fall=1
		p.falltime=ft
		p.currframe=0
		p.framecount=acount
		p.framecounttotal=acount
		p.existfor=1000
		Select t
			'Case	1
			'	p.image=
			Default
				p.image=invincibility_pickup
		End Select
		ListAddLast pickup_list,p
		Return p
	End Function
End Type

Global pickup:Tpickup



Type Tcoins
	Field x,y,fall,falltime,existfor
	Field currframe,framecount,framecounttotal
	Field image:TImage,value
	
	Method Update()
		If framecount=0 Then
			currframe=currframe+1
			If currframe>7 Then currframe=0
		End If
		framecount=framecount+1
		If framecount>=framecounttotal Then framecount=0
		
		If fall=1 Then
			If falltime=0 Then
				If CollideImage(coin_collide,x,y+2,0,1,0) Then fall=0
			Else
				falltime=falltime-1
				If falltime=0 And CollideImage(coin_collide,x,y,0,1,0) Then falltime=1
			End If
		End If
			
		
		If fall=1 Then y=y+4
		
		existfor=existfor-1
		
		If y>480 Or existfor<1 Then
			ListRemove coins_list,Self
			coins_list.remove(Self)
		Else
			CollideImage image,x,y,currframe,0,64,Self
		End If
		
	End Method
	
	Method Draw()
		SetBlend (MASKBLEND)
		DrawImage image,x,y,currframe
	End Method
	
	Function Spawn:Tcoins(px,py,acount,t,ft)
		Local c:Tcoins = New Tcoins
		c.x=px
		c.y=py
		c.fall=1
		c.falltime=ft
		c.currframe=0
		c.framecount=acount
		c.framecounttotal=acount
		c.existfor=1000
		Select t
			Case	1
				c.value=5
				c.image=spinning_coin_gold
			Default
				c.value=1
				c.image=spinning_coin_silver
		End Select
		ListAddLast coins_list,c
		Return c
	End Function
End Type

Global coins:Tcoins


Type Tgems
	Field x,y,fall,falltime,existfor
	Field currframe,framecount,framecounttotal
	Field image:TImage,value
	
	Method Update()
		If framecount=0 Then
			currframe=currframe+1
			If currframe>3 Then currframe=0
		End If
		framecount=framecount+1
		If framecount>=framecounttotal Then framecount=0
		
		If fall=1 Then
			If falltime=0 Then
				If CollideImage(gem_collide,x,y+2,0,1,0) Then fall=0
			Else
				falltime=falltime-1
				If falltime=0 And CollideImage(gem_collide,x,y,0,1,0) Then falltime=1
			End If
		End If

		
		If fall=1 Then
			y=y+4
		End If
		
		existfor=existfor-1
		
		If y>480 Or existfor<1 Then
			ListRemove gems_list,Self
			gems_list.remove(Self)
		Else
			CollideImage image,x,y,currframe,0,128,Self
		End If
		
	End Method
	
	Method Draw()
		SetBlend (MASKBLEND)
		DrawImage image,x,y,currframe
	End Method
	
	Function Spawn:Tgems(px,py,acount,t,ft)
		Local g:Tgems = New Tgems
		g.x=px
		g.y=py
		g.fall=1
		g.falltime=ft
		g.currframe=0
		g.framecount=0
		g.framecounttotal=acount
		g.existfor=1000
		Select t
			Case	1
				g.value=20
				g.image=gem_20_points
			Case	2
				g.value=40
				g.image=gem_40_points
			Case	3
				g.value=80
				g.image=gem_80_points
			Case	4
				g.value=100
				g.image=gem_100_points
			Case	5
				g.value=150
				g.image=gem_150_points
			Default
				g.value=10
				g.image=gem_10_points
		End Select
		ListAddLast gems_list,g
		Return g
	End Function
End Type

Global gems:Tgems



Type Tplayer
	Field x:Float,y:Float
	Field id,topspeed#,jump,lives,score,gems,coins,isdead,invincible,invflipflop
	Field jumpspeed#,speed#,grip#,skid#,zoom#
	Field canland,fall,canplacebomb,haslanded,onground,indoor
	Field currframe,animcount
	Field walkleft,walkright,facingdir,blink,blinkframe
	Field placingbomb,firepressed,weapon,num_mine,num_tnt,tnt_laid,cycledown,triggermine
	Field weaponfade#
	Field image:TImage



	Method Update()
		Local p
		Local joymoved,jumppressed,kickpressed,dropbomb,cyclepressed
		
		walkleft=0
		walkright=0
		If JoyDown(1,id)=0 And KeyDown(pfire[id])=0 Then firepressed=0
		If JoyDown(2,id)=0 And KeyDown(pcycle[id])=0 Then cycledown=0

		

		
		If isdead=0 And indoor=0 Then
			If facingdir=0 Then
				If id=0 Then	image=snipe_stand_left Else image=gripe_stand_left
			Else
				If id=0 Then	image=snipe_stand_right Else image=gripe_stand_right
			End If
		
			If jump=0 Then
				Local z=0
				Local oldfall:Int=fall
				For z=0 To 5	'How many lines to fall each frame
					fall=0				
					If Not CollideImage(spritemasks,x,y,0,COL_FOREG,0) Then
						y:+1
						canland=1
						onground=0
						fall=1
					Else
						If canland=0 Then
							fall=1
							y:+1
							onground=0
						Else
							If Not CollideImage(spritemasks,x,y-5,4,COL_FOREG,0) Then
								onground=1
							Else
								fall=1
								y:+1
								onground=0
							End If
						End If
					End If
				Next

'				For z=0 To 3
'					If Not CollideImage(spritemasks,x,y-1,0,1,0) Or CollideImage(spritemasks,x,y-3,0,1,0) Or y<-24 Then
'						y=y+1
'						fall=1
'					End If
'				Next

				If y>=480 Then y=-32
				If oldfall=1 And fall=0 Then  'Set vars to show player has landed a after jumping/falling
					haslanded=1
					animcount=-2
				End If
				'DrawImage spritemasks,x,y-1,0	
				'DrawImage spritemasks,x,y-3,0					
			End If

		
			jumppressed=0
			kickpressed=0
			dropbomb=0
			joymoved=0
			If joysticks>id Then
				If fall=0 Then
					If JoyX#(id)<-.2 Then
						joymoved=1
						walkleft=1		
					End If
					If (JoyHat#(id)=.750 Or JoyHat#(id)=.875 Or JoyHat#(id)=.625) And Abs(JoyX#(id))=<.2 Then
						joymoved=1
						walkleft=1
					End If
		
					If JoyX#(id)>.2 Then
						joymoved=1
						walkright=1
					End If
					If (JoyHat#(id)=.125 Or JoyHat#(id)=.250 Or JoyHat#(id)=.375) And Abs(JoyX#(id))<.2 Then
						joymoved=1
						walkright=1
					End If
				End If
				
					
				If JoyDown(0,id) Then jumppressed=1
				
				If JoyDown(1,id) And firepressed=0 Then
					kickpressed=1
					firepressed=1
				End If
				
				If JoyDown(2,id) And cycledown=0 Then cyclepressed=1
				
			EndIf
			
			If joymoved=0 Then
				If fall=0 Then
					If KeyDown(pleft[id]) Then
						walkleft=1
					Else If KeyDown(pright[id]) Then
						walkright=1
					End If
				End If
			
				If KeyDown(pjump[id]) Then jumppressed=1		
				
				If KeyDown(pfire[id]) And firepressed=0 Then
					kickpressed=1
					firepressed=1
				End If		
				
				If KeyDown(pcycle[id]) And cycledown=0 Then cyclepressed=1
			End If
		
			If placingbomb<>0 Then
				walkleft=0
				walkright=0
			End If	
			
			If walkleft=0 And walkright=0 Then
				If speed#>0 Then
					speed#=speed#-skid#
					If facingdir=0 Then
						If id=0 Then image=snipe_skid_halt_left Else image=gripe_skid_halt_left
					Else
						If id=0 Then image=snipe_skid_halt_right Else image=gripe_skid_halt_right
					End If
					If animcount=0 Then currframe=currframe+1
					If currframe>3 Then currframe=3
				End If
				If speed#<=.5 Then speed#=0
				
			End If
			
			If walkleft=1 Then
				blink=0
				If facingdir=0 Then
					speed#=speed#+grip#
					If speed#>topspeed# Then speed#=topspeed#			
					If animcount=0 Then currframe=currframe+1
					If currframe>=8 Then currframe=0
					If id=0 Then image=snipe_run_left Else image=gripe_run_left
				Else
					If speed#>=.5 Then
						If animcount=0 Then currframe=currframe+1
						speed#=speed#-grip#
						If speed#<.5 Then currframe=0
						If currframe>3 Then currframe=3
						If id=0 Then image=snipe_skid_halt_right Else image=gripe_skid_halt_right
					Else
						currframe=currframe+1
						If id=0 Then image=snipe_turn_right_to_left Else image=gripe_turn_right_to_left
						If currframe>3 Then
							facingdir=0
							If id=0 Then image=snipe_stand_left Else image=gripe_stand_left
							currframe=0
						End If
					End If
				End If
			End If
			If walkright=1 Then
				blink=0
				If facingdir=1 Then
					speed#=speed#+grip#
					If speed#>topspeed# Then speed#=topspeed#			
					If animcount=0 Then currframe=currframe+1
					If currframe>=8 Then currframe=0
					If id=0 Then image=snipe_run_right Else image=gripe_run_right
				Else
					If speed#>=.5 Then
						If animcount=0 Then currframe=currframe+1
						speed#=speed#-grip#
						If speed#<.5 Then currframe=0
						If currframe>3 Then currframe=3
						If id=0 Then image=snipe_skid_halt_left Else image=gripe_skid_halt_left
					Else
						currframe=currframe+1
						If id=0 Then image=snipe_turn_left_to_right Else image=gripe_turn_left_to_right
						If currframe>3 Then
							facingdir=1
							If id=0 Then image=snipe_stand_right Else image=gripe_stand_right
							currframe=0
						End If
					End If
				End If
			End If
			
			If facingdir=0 Then x=x-speed# Else x=x+speed#	
			
			If x>=642 Then x=-4
			If x<=-6 Then x=640
		
			If jumppressed And jump=0 And fall=0 And placingbomb=0 And Not CollideImage(image,x,y,0,512,0) And onground Then
				'Local jump_channel=CueSound(jump_snd)
				'SetChannelVolume (jump_channel,0.3)
				'ResumeChannel(jump_channel)
				If Not CollideImage(gripe_jump_left,x,y-8,0,COL_ROOF,0) Then
					PlaySound(jump_snd)
					jump=1
					blink=0
					jumpspeed#=-8			
					canland=0
					onground=0
					If walkleft Then
						speed#=topspeed#
						facingdir=0
					End If
					If walkright Then
						speed#=topspeed#
						facingdir=1
					End If
				Else
					jumppressed=0
				End If
			End If
		
	
			If jump Then
	'			DrawText y,100,100
	'			Flip
	'			WaitKey
				fall=0
				blink=0
				y=y+jumpspeed#
				If CollideImage(gripe_jump_left,x,y,0,COL_ROOF,0) Then
					jump=0
					y:-jumpspeed#
				Else
					If id=0 Then
						If facingdir=0 Then image=snipe_jump_left Else image=snipe_jump_right
					Else
						If facingdir=0 Then image=gripe_jump_left Else image=gripe_jump_right
					End If
					currframe=0
				End If
				'If jumppressed Then jumpspeed#=jumpspeed#+.32 Else jumpspeed#=jumpspeed#+.64
				jumpspeed#=jumpspeed#+.40
				
				If jump=1 And jumpspeed>0 Then	jump=2
	
				If jump=2 And Not CollideImage(spritemasks,x,y,0,COL_FOREG,0) Then canland=1
			
				'If jumpspeed#>=4 Then jumpspeed#=4
				If jumpspeed#>=4 Then jump=0
				If CollideImage(spritemasks,x,y,0,COL_FOREG,0) And canland=1 Then
					jump=0
				End If
				
	
	
	
			End If
			
			
			If walkleft=0 And walkright=0 And jump=0 And blink=0 And speed#=0 Then
				If Rand(500)=1 Then
					blink=1
					currframe=0
				End If
			End If
			
			If blink Then
				If facingdir=0 Then
					If id=0 Then image=snipe_blink_facing_left Else image=gripe_blink_facing_left
				Else
					If id=0 Then image=snipe_blink_facing_right Else image=gripe_blink_facing_right
				End If
				If animcount=0 Then currframe=currframe+1
				If currframe>7 Then
					blink=0
					currframe=0
				End If
			End If
			
			If triggermine Then
				If facingdir=0 Then
					If id=0 Then image=snipe_trigger_dynamite_left Else image=gripe_trigger_dynamite_left
				Else
					If id=0 Then image=snipe_trigger_dynamite_right Else image=gripe_trigger_dynamite_right
				End If
				
				If animcount=0 Then currframe=currframe+1
				If currframe=2 Then
					For Local tempd:Tdynamite = EachIn dynamite_list
						If tempd.belongsto=id Then
							Texplosion.Spawn(tempd.x,tempd.y-24,tempd.belongsto)
							ListRemove dynamite_list,tempd
						End If
					Next
				End If
				If currframe>7 Then
					currframe=0
					triggermine=0
					tnt_laid=0
				End If
			End If
			
			If placingbomb Then
				Select weapon
					Case 2
						If facingdir=0 Then
							If id=0 Then image=snipe_facing_left_drop_mine Else image=gripe_facing_left_drop_mine
						Else
							If id=0 Then image=snipe_facing_right_drop_mine Else image=gripe_facing_right_drop_mine
						End If
					Case 3
						If facingdir=0 Then
							If id=0 Then image=snipe_facing_left_drop_dynamite Else image=gripe_facing_left_drop_dynamite
						Else
							If id=0 Then image=snipe_facing_right_drop_dynamite Else image=gripe_facing_right_drop_dynamite
						End If
					Default
						If facingdir=0 Then
							If id=0 Then image=snipe_facing_left_drop_bomb Else image=gripe_facing_left_drop_bomb
						Else
							If id=0 Then image=snipe_facing_right_drop_bomb Else image=gripe_facing_right_drop_bomb
						End If
				End Select
				
				If animcount=0 Then currframe=currframe+1
				If currframe>3 Then
					placingbomb=0
					Select weapon
						Case 2
							If facingdir=0 Then
								Tmine.spawn(x-6,y+8,0,3,4,id)
								Else
								Tmine.spawn(x+4,y+8,0,3,4,id)
							End If		
							num_mine:-1	
						Case 3
							If tnt_laid=0 Then
								If facingdir=0 Then
									Tdynamite.spawn(x-5,y+8,0,3,4,id)
									Else
									Tdynamite.spawn(x+7,y+8,0,3,4,id)
								End If
								tnt_laid=1
								num_tnt:-1
							End If
						Default					
							If facingdir=0 Then
							Tbomb.spawn(x-7,y+8,-speed#,grip#,200,0,3,4,id)
							Else
							Tbomb.spawn(x+4,y+8,speed#,grip#,200,0,3,4,id)
						End If
					End Select				
					currframe=0
				End If
			End If
			
			If cyclepressed And placingbomb=0 Then
				weapon=weapon+1
				If weapon>3 Then weapon=1
				cycledown=1
				weaponfade#=1
			End If
			
			If haslanded=1 Then
				If id=0 Then
					If facingdir=0 Then image=snipe_skid_halt_left Else image=snipe_skid_halt_right
				Else
					If facingdir=0 Then image=gripe_skid_halt_left Else image=gripe_skid_halt_right
				End If
				currframe=2
			End If
			
			If fall=1 Then
				If id=0 Then
					If facingdir=0 Then image=snipe_jump_left Else image=snipe_jump_right
				Else
					If facingdir=0 Then image=gripe_jump_left Else image=gripe_jump_right
				End If
				currframe=0
			End If
			
			If image=snipe_stand_left Or image=gripe_stand_left Or image=snipe_stand_right Or image=gripe_stand_right Then currframe=0

			'Test collision with lefthand boundary
			Local checkpos:Int=y+8
			If checkpos<1 Then checkpos=1
			If CollideImage(spritemasks,x,checkpos,0,COL_LEFTH,0) Then
				'x=x+topspeed#
				x:+speed#
				speed#=0
			End If
			'Test collision with righthand boundary
			If CollideImage(spritemasks,x,checkpos,0,COL_RIGHTH,0) Then
				'x=x-topspeed#
				x:-speed#
				speed#=0
			End If
			
			'DrawImage spritemasks,x,checkpos,0
			
			'Collision detection for enemies
			If CollideImage(image,x,y,currframe,COL_ENEMY,0) And invincible=0 Then isdead=1
			

			Local cc:Object[]
			'Collision detection for Gems
			cc=CollideImage(image,x,y,currframe,COL_GEM,0)
			If cc Then
				For Local tempgem:Tgems = EachIn gems_list
					If tempgem=cc[0] Then
						score=score+tempgem.value
						Treward.Spawn(x+4,y,tempgem.value)
						Tpickedup16.Spawn(tempgem.x,tempgem.y)
						gems=gems+1
						If gems>99 Then gems=99
						ListRemove gems_list,tempgem
						PlaySound collect_snd
					End If
				Next
			End If


			
			'Collision detection for Coins
			cc=CollideImage(image,x,y,currframe,COL_COIN,0)
			If cc Then
				For Local tempcoin:Tcoins = EachIn coins_list
					If tempcoin=cc[0] Then
						coins=coins+tempcoin.value
						Tpickedup16.Spawn(tempcoin.x,tempcoin.y)
						If coins>99 Then coins=99
						ListRemove coins_list,tempcoin
						PlaySound collect_snd
					End If
				Next
			End If
			
			'Collision detection for pick-ups
			cc=CollideImage(image,x,y,currframe,COL_PICKUP,0)
			If cc Then
				For Local temppickup:Tpickup = EachIn pickup_list
					If temppickup=cc[0] Then
						Tpickedup32.Spawn(temppickup.x,temppickup.y)
						ListRemove pickup_list,temppickup
						PlaySound collect_snd
					End If
				Next
			End If
			
			'Collision detection for mines
			cc=CollideImage(image,x,y,currframe,COL_MINE,0)
			If cc Then
				For Local tempmine:Tmine = EachIn mine_list
					If tempmine=cc[0] Then
						If tempmine.time=0 Then
							Texplosion.Spawn(tempmine.x,tempmine.y-24,tempmine.belongsto)
							ListRemove mine_list,tempmine
						End If
					End If
				Next
			End If
			
			'Collision detection for Exploding bombs
			cc=CollideImage(image,x,y,currframe,COL_EXPLOSION,0)
			If cc And invincible=0 Then
				'For Local tempbomb:Tbomb = EachIn bomb_list
				'	If tempbomb=cc[0] Then
				'		whosbomb?=tempbomb=id
				'	End If
				'Next
				isdead=1
			End If
			
			'Collision detection for unexploded Bombs/for bomb kick routine
			cc=CollideImage(image,x,y,currframe,16,0)
			If cc Then
				If kickpressed And fall=0 And jump=0 Then
					For Local tempbomb:Tbomb = EachIn bomb_list
						If tempbomb=cc[0] And tempbomb.kicked=False Then
							tempbomb.dy#=-2
							If facingdir=0 Then tempbomb.dx#=-2 Else tempbomb.dx#=2
							tempbomb.kicked=True
						End If
					Next
				End If
			Else
				If kickpressed Then
					If weapon=3 And tnt_laid=1 Then
						If jump=0 And triggermine=0 And fall=0 Then
							triggermine=1
							currframe=0
						End If
					Else If fall=0 And jump=0 And canplacebomb=0
						If weapon=1 Or speed#=0 Then
							If (weapon=2 And num_mine>0) Or (weapon=3 And num_tnt>0) Or weapon=1 Then
								placingbomb=1
								canplacebomb=40
								currframe=0
							End If
						End If
					End If
					firepressed=1
				End If		
			End If



			If invincible>0 Then
				invincible=invincible-1
				invflipflop=1-invflipflop
			Else
				invincible=0
				invflipflop=0
			End If
			If canplacebomb>0 Then canplacebomb=canplacebomb-1
			
			animcount=animcount+1
			If animcount>2 Then
				animcount=0
				haslanded=0
			End If
		
		End If	
		
		If isdead=1 Then	'Set up start of death sequence
			isdead=2
			currframe=0
			If id=0 Then image=snipe_die Else image=gripe_die
			animcount=0
			zoom#=1
		End If
			
		If isdead=2 Then	'process player death sequence
			animcount:+1
			If animcount>2 Then animcount=0
			If animcount=0 Then currframe:+1
			If currframe>3 Then
				currframe=3
				isdead=3
			End If
			y#:-2
			zoom#:+.01
		End If
		
		If isdead=3 Then
			animcount:+1
			If animcount>2 Then animcount=0
			If animcount=0 Then currframe:+1
			If currframe>3 Then currframe=0
			y#:+8
			If y#>496 Then isdead=4
		End If
		
		If isdead=4 Then		' **** Reset player after death!
			If id=0 Then
				x=level.p1x
				y=level.p1y
			Else
				x=level.p2x
				y=level.p2y
			End If
			jump=0
			fall=0
			speed#=0
			canplacebomb=0
			isdead=0
			lives=lives-1
			invincible=150
			currframe=0
			If lives=0 Then
				Local hpos:Byte=CheckScore(score)
				If hpos<10 Then THiScoreTable.Spawn(id,score,hpos)
				ListRemove player_list,Self
				player_list.remove(Self)
			End If
			
		End If

	End Method
	
	Method CheckExit()
		If CollideImage(image,x,y,currframe,COL_DOOR,0) And indoor=0 Then
			If x>=level.door_x-1 And x<=level.door_x+1 And y>=level.door_y-16 And y<=level.door_y+18 Then
				indoor=1
				level.playersin:+1
			End If
		End If
	End Method
	
	Method Draw()
		SetBlend (MASKBLEND)
		SetColor 255,255,255
		'SetScale (-1,1)
			
		CollideImage image,x,y,currframe,0,256,Null
		If invflipflop=0 Then
			If isdead>0 Then SetScale(zoom#,zoom#)
			If Not indoor Then DrawImage image,x,y,currframe
			If weaponfade#>0 Then
				SetBlend(ALPHABLEND)
				SetAlpha(weaponfade#)
				Select weapon
					Case 2
						DrawImage mine_floor,x,y-22,0
					Case 3
						DrawImage dynamite_floor,x,y-22,0
					Default
						DrawImage bomb_floor,x,y-22,0
				End Select
				SetBlend(MASKBLEND)
				SetAlpha(1)
				weaponfade#=weaponfade#-.02
			End If
			SetScale(1,1)
		End If


		'SetScale (1,1)
	End Method
	
	Method Appear()
		SetBlend (ALPHABLEND)
	End Method
	
	Method ShowStats()
		'If weapon=2 And num_mine=0 Then weapon=3
		'If weapon=3 And num_tnt=0 Then weapon=1
		Local sx
		If id=0 Then sx=8 Else sx=536
		Text(PutNumberLeadingZeros(score,6),sx,8)
		If id=0 Then
			sx=sx+16
			DrawImage icons,sx+8,40,0
		Else
			sx=sx+16
			DrawImage icons,sx+8,40,1
		End If
		SmallText(PutNumberLeadingZeros(lives,2),sx,48)
		DrawImage icons,sx+32,40,2
		SmallText(PutNumberLeadingZeros(gems,2),sx+24,48)
		DrawImage icons,sx+56,40,3
		SmallText(PutNumberLeadingZeros(coins,2),sx+48,48)

		If weapon<>1 Then
			SetBlend(ALPHABLEND)
			SetAlpha(0.3)
		Else
			SetBlend(MASKBLEND)
			SetAlpha(1)
		End If		
		DrawImage bomb_floor,sx+8,66,0
		If weapon<>2 Then
			SetBlend(ALPHABLEND)
			SetAlpha(0.3)
		Else
			SetBlend(MASKBLEND)
			SetAlpha(1)
		End If
		DrawImage mine_floor,sx+32,66,0
		SmallText(num_mine,sx+28,63)
		If weapon<>3 Then
			SetBlend(ALPHABLEND)
			SetAlpha(0.3)
		Else
			SetBlend(MASKBLEND)
			SetAlpha(1)
		End If
		DrawImage dynamite_floor,sx+56,66,0
		SmallText(num_tnt,sx+52,63)
		SetBlend(MASKBLEND)
		SetAlpha(1)		
	End Method
	
	Method InitialPosition()
		If id=0 Then
			x=level.p1x
			y=level.p1y
		Else
			x=level.p2x
			y=level.p2y
		End If
		indoor=0
		jump=0
		speed=0
		tnt_laid=0
	End Method
	
	Function Spawn:Tplayer(i,px,py,s,j,b,l,fd)
		Local p:Tplayer = New Tplayer
		p.id=i
		p.x=px
		p.y=py
		p.topspeed#=s
		p.speed#=0
		p.jump=j
		p.canplacebomb=b
		p.lives=l
		p.coins=0
		p.gems=0
		p.facingdir=fd
		p.isdead=0
		p.grip#=.2
		p.skid#=.08
		p.weapon=1
		p.weaponfade#=0
		p.num_mine=1
		p.num_tnt=1
		p.tnt_laid=0
		p.canland=1
		p.haslanded=0
		ListAddLast player_list,p
		Return p
	End Function

End Type

Global player:Tplayer



Type THiScoreTable
	Field id:Int
	Field score,position:Int
	Field entrytime:Int
	Field hstring:String
	Field alpha:Float,adir:Float
	Field blink:Byte,blinkcount:Byte
	Field currletter:Byte,entrycount:Byte
	
	Method Enter()
		Local x:Int
		Select id	'Set position for Hi-score entry to be displayed
		Case 0
			x=8
		Case 1
			x=536
		End Select
		
		alpha:+adir
		If alpha<=0 Or alpha>=1 Then
			adir=-adir
			alpha:+adir
		End If
		SetBlend(ALPHABLEND)
		SetAlpha(alpha)
		Text(PutNumberLeadingZeros(score,6),x,8)		
		SetAlpha(1)
		SetBlend(MASKBLEND)
		If currletter<>0 Or blink=0 Then Text(hstring[0..1],x+24,28)
		If currletter<>1 Or blink=0 Then Text(hstring[1..2],x+40,28)
		If currletter<>2 Or blink=0 Then Text(hstring[2..3],x+56,28)
		
		blinkcount=(1+blinkcount) Mod 8
		If blinkcount=0 Then blink=Not blink
		
		If entrycount>0 Then entrycount:-1
		
		Local joymoved:Byte=0
		Local lleft:Byte=0
		Local lright:Byte=0
		If entrycount=0 And currletter<3 Then
			If JoyX#(id)<-.2 Then
				joymoved=1
				lleft=1		
			End If
			If (JoyHat#(id)=.750 Or JoyHat#(id)=.875 Or JoyHat#(id)=.625) And Abs(JoyX#(id))=<.2 Then
				joymoved=1
				lleft=1
			End If
		
			If JoyX#(id)>.2 Then
				joymoved=1
				lright=1
			End If
			If (JoyHat#(id)=.125 Or JoyHat#(id)=.250 Or JoyHat#(id)=.375) And Abs(JoyX#(id))<.2 Then
				joymoved=1
				lright=1
			End If
			
			If joymoved=0 Then
				If KeyDown(pleft[id]) Then
					lleft=1
				Else If KeyDown(pright[id]) Then
					lright=1
				End If
			End If
			
			If JoyDown(0,id) Or KeyDown(pfire[id]) Then
				currletter:+1
				lleft=0
				lright=0
				entrycount=8
				If currletter=3 Then entrytime=400
			End If
			
		End If
		
		If lleft=1 And lright=1 Then
			lleft=0
			lright=0
		End If
		
		If lleft=1 Then
			Local a:Byte[3]
			a[0]=Asc(hstring[0..1])
			a[1]=Asc(hstring[1..2])
			a[2]=Asc(hstring[2..3])
			a[currletter]:-1
			If a[currletter]<32 Then a[currletter]=90
			hstring=Chr(a[0])+Chr(a[1])+Chr(a[2])
			entrycount=8
		End If
		
		If lright=1 Then
			Local a:Byte[3]
			a[0]=Asc(hstring[0..1])
			a[1]=Asc(hstring[1..2])
			a[2]=Asc(hstring[2..3])
			a[currletter]:+1
			If a[currletter]>90 Then a[currletter]=32
			hstring=Chr(a[0])+Chr(a[1])+Chr(a[2])
			entrycount=8
		End If

		
		
		entrytime:-1
		If entrytime<=0 Then
			hiscore[position]=score
			hiname[position]=hstring
			hinew[position]=1
			ListRemove HiScoreTable_list,Self
			HiScoreTable_list.remove(Self)
		End If
		
	End Method
	

	Function Spawn:THiScoreTable(i,s,p)
		Local h:THiScoreTable = New THiScoreTable
		h.id=i
		h.score=s
		h.entrytime=5000
		h.hstring="AAA"
		h.alpha=1
		h.adir=-.04
		h.blinkcount=9
		h.blink=0
		h.currletter=0
		h.entrycount=0
		h.position=p
		ListAddLast HiScoreTable_list,h
		Return h
	End Function


End Type
Global HiScoreTable:THiScoreTable

Type Tlevel
	Field mainscroll,mainscroll2,id
	Field backdrop:TImage,middrop:TImage
	Field p1x,p1y,p2x,p2y
	Field num_enemy,is_complete,num_teleport
	Field playersin,door_appear,door_x,door_y,door_timer,door_frame '(heh)
	
	Method Play()
		Local x,y,l
		Local timing:Int=MilliSecs()

		SetColor 255,255,255
		
		If background_on=1 Then 
			SetBlend(SOLIDBLEND)
			DrawImage backdrop,0,mainscroll,0
			DrawImage backdrop,0,mainscroll+480,0
			SetBlend (MASKBLEND)
			'SetBlend (ALPHABLEND)
			'SetAlpha (.5)
			DrawImage middrop,0,mainscroll2,0
			DrawImage middrop,0,mainscroll2+480,0
			'SetBlend (MASKBLEND)
			'SetAlpha (1)
		Else
			Cls
		End If

		mainscroll=mainscroll+1
		If mainscroll>0 Then mainscroll=-479
		mainscroll2=mainscroll2+2
		If mainscroll2>0 Then mainscroll2=-478


		ResetCollisions(0)
		SetBlend (MASKBLEND)
		For y=0 To 29
			For x=0 To 39
				If map[0,x,y]>0 Then
					DrawImage tilesBitmap,x*16,y*16,map[0,x,y]
				End If
				If map[1,x,y]>0 Then
					DrawImage tilesBitmap,x*16,y*16,map[1,x,y]
				End If
				If map[3,x,y]>0 Then
					If map[3,x,y]=2 Then CollideImage tilesBitmap,x*16,y*16,map[1,x,y],0,COL_LEFTH,Null
					If map[3,x,y]=4 Then CollideImage tilesBitmap,x*16,y*16,map[1,x,y],0,COL_ROOF,Null
					If map[3,x,y]=8 Then CollideImage tilesBitmap,x*16,y*16,map[1,x,y],0,COL_FOREG,Null
					If map[3,x,y]=16 Then CollideImage tilesBitmap,x*16,y*16,map[1,x,y],0,COL_RIGHTH,Null
					If map[3,x,y]&96 Then CollideImage tilesBitmap,x*16,y*16,map[1,x,y],0,COL_ENEMY,Null
					'DrawRect x*16+4,y*16+4,8,8
				End If
			Next
		Next
	


		SetColor 255,255,255	
		SetBlend (MASKBLEND)
		
		If Rand(1000)=1 Then					'Randomly drop coins
			Tcoins.spawn(Rand(524)+50,-14,4,Rand(0,1),Rand(90)+10)
		End If	
		
		If Rand(1000)=1 Then					'Randomly drop gems
			Tgems.spawn(Rand(524)+50,-16,4,Rand(0,5),Rand(90)+10)
		End If	
		
		If Rand(1000)=1 Then					'Randomly drop pickups
			Tpickup.spawn(Rand(524)+50,-16,4,Rand(0,0),Rand(90)+10)
		End If


		If door_appear=2 Then				'Draw door frame under player
			DrawImage exitpost_back,door_x,door_y,door_frame
			CollideImage exitpost_back,door_x,door_y,door_frame,0,COL_DOOR,Null
			CollideImage exitpost_overlay,door_x,door_y,door_frame,0,COL_DOOR,Null
			CollideImage exitpost_collision,door_x,door_y-18,0,COL_FOREG,Null
			door_timer=door_timer+1
			If door_timer>=2 Then door_timer=0
			If door_timer=0 Then door_frame=door_frame+1
			If door_frame=8 Then door_frame=0
		End If

		For DestructTile:TDestructTile = EachIn DestructTile_list		'Process destructable tiles
			DestructTile.AddtoCollisionLayer()
		Next

		For explosion:Texplosion = EachIn explosion_list
			explosion.Update()
			explosion.Draw()
		Next
			
		For bomb:Tbomb = EachIn bomb_list		'Process bombs
			bomb.Update()
		Next
		
		For mine:Tmine = EachIn mine_list		'Process mines
			mine.Update()
			mine.Draw()
		Next
		
		For wheelie:Twheelie = EachIn wheelie_list	'Process/draw wheelies
			wheelie.Update()
			wheelie.Draw()
		Next
		
		For grog:Tgrog = EachIn grog_list			'Process/draw grogs
			grog.Update()
			grog.Draw()
		Next
		
		For slinky:Tslinky = EachIn slinky_list			'Process/draw slinkys
			slinky.Update()
			slinky.Draw()
		Next


		
		For coins:Tcoins = EachIn coins_list	'Process/draw coins
			coins.Update()
			coins.Draw()
		Next
		
		For gems:Tgems = EachIn gems_list		'Process/draw gems
			gems.Update()
			gems.Draw()
		Next
		
		For pickup:Tpickup = EachIn pickup_list		'Process/draw gems
			pickup.Update()
			pickup.Draw()
		Next

				
		For player:Tplayer = EachIn player_list	'Process/draw player
			player.Update()
			player.CheckExit()
			player.Draw()
			player.ShowStats()
		Next
		
		
		For pickedup16:Tpickedup16 = EachIn pickedup16_list		'Process/draw pick up anims
			pickedup16.Update()
			pickedup16.Draw()
		Next
		
		For pickedup32:Tpickedup32 = EachIn pickedup32_list		'Process/draw pick up anims
			pickedup32.Update()
			pickedup32.Draw()
		Next
		
		If door_appear=1 Then				'Draw door appearing animation
			DrawImage exitpost_appear,door_x,door_y,door_frame
			door_timer=door_timer+1
			If door_timer>=1 Then door_timer=0
			If door_timer=0 Then door_frame=door_frame+1
			If door_frame=40 Then
				door_frame=0
				door_appear=2
			End If
		End If
		
		If door_appear=3 Then				'Draw door appearing animation
			DrawImage exitpost_disappear,door_x,door_y,door_frame
			door_timer=door_timer+1
			If door_timer>=4 Then door_timer=0
			If door_timer=0 Then door_frame=door_frame+1
			If door_frame=32 Then
				door_frame=0
				door_appear=4
			End If
		End If
		
		
		For dynamite:Tdynamite = EachIn dynamite_list		'Process dynamite
			dynamite.Update()
			dynamite.Draw()
		Next


		
		For reward:Treward = EachIn reward_list
			reward.Update()
			reward.Draw()
		Next

			
		For bomb:Tbomb = EachIn bomb_list		'Draw bombs
			bomb.Draw()
		Next
		SetBlend (MASKBLEND)

		
		For smoke:Tsmoke = EachIn smoke_list
			smoke.Update()
			smoke.Draw()
		Next

		If door_appear=2 Then
			DrawImage exitpost_overlay,door_x,door_y,door_frame	'Draw door overlay over player
		End If
		
		If level.playersin>=CountList(player_list) And door_appear=2 Then
			door_appear=3
			door_frame=0
			door_timer=0
		End If
		
		SetBlend (MASKBLEND)
		For y=0 To 29
			For x=0 To 39
				If map[2,x,y]>0 Then
					DrawImage tilesBitmap,x*16,y*16,map[2,x,y]
				End If
			Next
		Next
		
		For HiScoreTable:THiScoreTable = EachIn HiScoreTable_list
			HiScoreTable.Enter()
		Next


		
		If CountList(wheelie_list)=0 And CountList(grog_list)=0 And CountList(slinky_list)=0 And door_appear=0 Then
			door_appear=1
			door_frame=0
			door_timer=0
		End If
		
		timing=calctime(MilliSecs()-timing)
		
		text("MSECS: "+timing,0,460)
		
		
		

		
	End Method
	
	Method AddEnemies()
		Local offset=8+(id * LEVEL_SIZE)+16
		num_enemy=PeekInt(levelbank,offset)			'Retrieve num of enemies
		offset=offset+4			'set offset to 1st enemy data
		Local z
		For z=0 To 7
			If PeekInt(levelbank,offset+16) Then
				Select PeekInt(levelbank,offset+12)
				Case 0
					Twheelie.spawn(PeekInt(levelbank,offset),PeekInt(levelbank,offset+4),PeekInt(levelbank,offset+8))
				Case 1
					Tgrog.spawn(PeekInt(levelbank,offset),PeekInt(levelbank,offset+4),PeekInt(levelbank,offset+8))
				Case 2
					Tslinky.spawn(PeekInt(levelbank,offset),PeekInt(levelbank,offset+4),PeekInt(levelbank,offset+8))
				End Select
			End If
			offset=offset+20
		Next
	End Method
	
	Method  LoadMusic(track)	'Load music track from memory
		Local r
		Select track
			Case 1
				r=BASSMOD_MusicLoad(True,tune1add,0,BankSize(tune1),0)
			Case 2
				r=BASSMOD_MusicLoad(True,tune2add,0,BankSize(tune2),0)
			Case 3
				r=BASSMOD_MusicLoad(True,tune3add,0,BankSize(tune3),0)
			Case 4
				r=BASSMOD_MusicLoad(True,tune4add,0,BankSize(tune4),0)
			Default
				r=-1			
		End Select
		If r=0 Then
			Local e=BASSMOD_ErrorGetCode()
			Notify("Error in Audio Module.  Error Code "+e+".",True)
			End
		End If
	End Method
	
	
	Method Fadein()
		Local x,y,c,showenemy
		c=0
		showenemy=0
		Local scale#=0
		While scale#<1

			SetColor c,c,c
	
		If background_on=1 Then 
			SetBlend(SOLIDBLEND)
			DrawImage backdrop,0,mainscroll,0
			DrawImage backdrop,0,mainscroll+480,0
			SetBlend (MASKBLEND)
			'SetBlend (ALPHABLEND)
			'SetAlpha (.5)
			DrawImage middrop,0,mainscroll2,0
			DrawImage middrop,0,mainscroll2+480,0
			'SetBlend (MASKBLEND)
			'SetAlpha (1)
		Else
			Cls
		End If

		mainscroll=mainscroll+1
		If mainscroll>0 Then mainscroll=-479
		mainscroll2=mainscroll2+2
		If mainscroll2>0 Then mainscroll2=-478



			SetBlend (MASKBLEND)
			For y=0 To 29
				For x=0 To 39
					If map[0,x,y]>0 Then
						DrawImage tilesBitmap,x*16,y*16,map[0,x,y]
					End If
					If map[1,x,y]>0 Then
						DrawImage tilesBitmap,x*16,y*16,map[1,x,y]
					End If
					If map[2,x,y]>0 Then
						DrawImage tilesBitmap,x*16,y*16,map[2,x,y]
					End If
				Next
			Next
			
			If showenemy Then
				For wheelie:Twheelie = EachIn wheelie_list			'Process/draw wheelies
					wheelie.Appear(scale#)
				Next
				For grog:Tgrog = EachIn grog_list			'Process/draw wheelies
					grog.Appear(scale#)
				Next
				For slinky:Tslinky = EachIn slinky_list			'Process/draw wheelies
					slinky.Appear(scale#)
				Next
				scale#=scale#+.1
			End If

			Flip
			c=c+4
			If c>255 Then
				c=255
				showenemy=1
			End If
		Wend
	End Method


	Method Fadeout()					'Fade level out
		Local x,y,c,v
		c=255						'Set initial colour intensity
		v=50							'Set inital volume
		While c>0					'Loop until colour faded to black

			SetColor c,c,c				'Set colour drawing intensity
	
		If background_on=1 Then 
			SetBlend(SOLIDBLEND)
			DrawImage backdrop,0,mainscroll,0
			DrawImage backdrop,0,mainscroll+480,0
			SetBlend (MASKBLEND)
			'SetBlend (ALPHABLEND)
			'SetAlpha (.5)
			DrawImage middrop,0,mainscroll2,0
			DrawImage middrop,0,mainscroll2+480,0
			'SetBlend (MASKBLEND)
			'SetAlpha (1)
		Else
			Cls
		End If


		mainscroll=mainscroll+1
		If mainscroll>0 Then mainscroll=-479
		mainscroll2=mainscroll2+2
		If mainscroll2>0 Then mainscroll2=-478



			SetBlend (MASKBLEND)
			For y=0 To 29
				For x=0 To 39
					If map[0,x,y]>0 Then
						DrawImage tilesBitmap,x*16,y*16,map[0,x,y]
					End If
					If map[1,x,y]>0 Then
						DrawImage tilesBitmap,x*16,y*16,map[1,x,y]
					End If
					If map[2,x,y]>0 Then
						DrawImage tilesBitmap,x*16,y*16,map[2,x,y]
					End If
				Next
			Next

			Flip
			c=c-4						'Reduce colour intensity variable
			v=v-2						'Reduce MOD volume variable
			If v<0 Then v=0
			If soundpresent Then BASSMOD_SetVolume(v)			'Set MOD volume
			If c<0 Then c=0
		Wend
		If soundpresent Then BASSMOD_MusicStop()			'Stop MOD playing
		If soundpresent Then BASSMOD_MusicFree()
	End Method

	Method ScrollToNext()
		Local z,x,y,i
		Local row_count=0
		Local trans_row=0
		Local trans_scroll=0
		Local tempmap[3,40,30]
		For z=0 To 2
			For y=0 To 29
				For x=0 To 39
					tempmap[z,x,y]=map[z,x,y]
				Next
			Next
		Next
		id:+1
		Create(id)
		While row_count<30
			If background_on=1 Then 
				SetBlend(SOLIDBLEND)
				DrawImage backdrop,0,mainscroll,0
				DrawImage backdrop,0,mainscroll+480,0
				SetBlend (MASKBLEND)
				DrawImage middrop,0,mainscroll2,0
				DrawImage middrop,0,mainscroll2+480,0
			Else
					Cls
			End If

			mainscroll=mainscroll+1
			If mainscroll>0 Then mainscroll=-479
			mainscroll2=mainscroll2+2
			If mainscroll2>0 Then mainscroll2=-478

			i=0
			For y=row_count To 29
				For x=0 To 39
					For z=0 To 2
						If tempmap[z,x,y]>0 Then DrawImage tilesBitmap,x*16,i*16-trans_scroll,tempmap[z,x,y]
					Next
				Next
				i:+1	
			Next
			For y=0 To row_count
				For x=0 To 39
					For z=0 To 2
						If map[z,x,y]>0 Then DrawImage tilesBitmap,x*16,i*16-trans_scroll,map[z,x,y]
					Next
				Next
				i:+1	
			Next
			Flip
			trans_scroll:+4
			If trans_scroll>15 Then
				row_count:+1
				trans_scroll=0
			End If
			'WaitKey
		Wend	
	End Method
	
	
	Method Create(l)						'Create level
		Local x,y,offset,i
		
		id=l								'Store level id
		num_enemy=0
		door_appear=0
		playersin=0
		
		'**** Storage of mapdata will be using a Blitz Bank in due course ****

		offset=8+(id * LEVEL_SIZE)+232		'calculate offset for layer 0
		i=0
		For y=0 To 29	
			For x=0 To 39
				map[0,x,y]=PeekByte(levelbank,offset+i)			'Read in map data
				i=i+1
			Next
		Next

		offset=8+(id * LEVEL_SIZE)+1432		'calculate offset for layer 1
		i=0
		For y=0 To 29	
			For x=0 To 39
				map[1,x,y]=PeekByte(levelbank,offset+i)			'Read in map data
				i=i+1
			Next
		Next

		
		offset=8+(id * LEVEL_SIZE)+2632		'calculate offset for layer 2
		i=0
		For y=0 To 29	
			For x=0 To 39
				map[2,x,y]=PeekByte(levelbank,offset+i)			'Read in map data
				i=i+1
			Next
		Next

		offset=8+(id * LEVEL_SIZE)+3832		'calculate offset for tile boundary
		i=0
		For y=0 To 29
			For x=0 To 39
				map[3,x,y]=PeekByte(levelbank,offset+i)		'Read in bounds data
				If map[3,x,y]&192 Then TDestructTile.Spawn(x,y,map[1,x,y])
				i=i+1
			Next
		Next
		
		SetBlend (solidblend)
		offset=8+(id * LEVEL_SIZE)
		Local bimage=PeekInt(levelbank,offset+196)
		Select bimage
		Case 0
			backdrop=backscreen0				'Draw picture for backdro
			middrop=backscreen1
		Default
			backdrop=backscreen0				'Draw picture for backdro
			middrop=backscreen1
		End Select
		
		
		
		p1x=PeekInt(levelbank,offset+0)		'Load initial player start positions
		p1y=PeekInt(levelbank,offset+4)
		p2x=PeekInt(levelbank,offset+8)
		p2y=PeekInt(levelbank,offset+12)
		door_x=PeekInt(levelbank,offset+180)	'Load position for door to appear
		door_y=PeekInt(levelbank,offset+184)
		
	End Method
	
End Type


' **** END OF TYPE DECLARATIONS ****


SeedRnd MilliSecs()


Global startlevel=0
Global quitgame=0
Global joysticks=JoyCount()					'Count number of joysticks attached
Global level:Tlevel=New Tlevel					'Create of global instance of level
'level.backdrop=CreateImage(640,480,DYNAMICIMAGE)	'Set level backdrop image
'level.middrop=CreateImage(640,480,DYNAMICIMAGE)	'Set level backdrop image


While quitgame=0
	SetBlend (MASKBLEND)
	SetColor 255,255,255
	'If soundpresent Then BASSMOD_SetVolume(50)						'Set music volume
	If soundpresent Then BASSMOD_MusicLoad(True,tune0add,0,BankSize(tune0),0)
	If soundpresent Then BASSMOD_MusicPlayEx(0,BASS_MUSIC_LOOP,False)
'	Local cloud_scroll:Int=0
'	Local mountain_scroll:Int=0
	Local intro_scroll:Int=0
	Local intro_frame:Int=0
	Local intro_count:Int=0
	Local intro_option:Int=0
	Local max_option:Int=3
	Local show_bomb:Int=1
	Local exit_intro:Int=0
	Local bombxpos:Int
	Local whichmenu:Int=0
	Local definekey:Int=0
	Local noaction:Int=0
	FlushKeys
	While exit_intro<2
		ResetCollisions(0)
		DrawImage intro_screen_scroll,intro_scroll,96
		DrawImage intro_screen_scroll,intro_scroll+1280,96
		DrawImage intro_header,0,0
		DrawImage intro_footer,0,448

		'DrawImage intro_clouds,cloud_scroll,0
		'DrawImage intro_mountain,mountain_scroll,200
		'DrawImage intro_floor,floor_scroll,368
		'DrawImage intro_logo,48,16
		intro_scroll:-4
		If intro_scroll<=-1280 Then intro_scroll=0
		'mountain_scroll:-2
		'If mountain_scroll<=-512 Then mountain_scroll=0
		'cloud_scroll:-1
		'If cloud_scroll<=-512 Then cloud_scroll=0	
		
		DrawImage intro_icons,32,160,0
		CollideImage intro_icons,32,160,6,0,1,Null
		DrawImage intro_icons,192,160,1
		CollideImage intro_icons,192,160,6,0,2,Null
		DrawImage intro_icons,352,160,2
		CollideImage intro_icons,352,160,6,0,4,Null
		DrawImage intro_icons,512,160,3
		CollideImage intro_icons,512,160,6,0,8,Null
		
		DrawImage intro_icons,32,300,4
		CollideImage intro_icons,32,300,6,0,16,Null
		DrawImage intro_icons,192,300,5
		CollideImage intro_icons,192,300,6,0,32,Null
		DrawImage intro_icons,352,300,7
		CollideImage intro_icons,352,300,6,0,64,Null
		DrawImage intro_icons,512,300,6
		CollideImage intro_icons,512,300,6,0,128,Null
		
		
		
		
		Local mx:Int=MouseX()
		Local my:Int=MouseY()
		DrawImage mouse_pointer,mx,my
		
		
		If CollideImage (mouse_pointer,mx,my,0,1,0) Then Text("SINGLE PLAYER"),216,462
		If CollideImage (mouse_pointer,mx,my,0,2,0) Then Text("TWO PLAYER TEAM"),200,462
		'If CollideImage (mouse_pointer,mx,my,0,4,0) Then Text("TWO PLAYER VERSUS"),184,462
		'If CollideImage (mouse_pointer,mx,my,0,8,0) Then Text("SINGLE PLAYER"),216,462
		If CollideImage (mouse_pointer,mx,my,0,128,0) Then Text("EXIT GAME"),248,462
		

		If MouseDown(1) And CollideImage (mouse_pointer,mx,my,0,1,0) Then
			exit_intro=2
			intro_option=0
		End If


		If MouseDown(1) And CollideImage (mouse_pointer,mx,my,0,2,0) Then
			exit_intro=2
			intro_option=1
		End If
		
		
		If MouseDown(1) And CollideImage (mouse_pointer,mx,my,0,128,0) Then
			exit_intro=2
			intro_option=7
		End If
		
		intro_count=(1+intro_count) Mod 5
		If intro_count=0 Then
			intro_frame=(1+intro_frame) Mod 4
			If exit_intro=0 And definekey=0 Then
				If KeyDown(KEY_UP) Then
					intro_option:-1
					If intro_option<0 Then intro_option=max_option
					noaction=0
				End If
				If KeyDown(KEY_DOWN) Then
					intro_option:+1
					If intro_option>max_option Then intro_option=0
					noaction=0
				End If
				If KeyDown(KEY_LEFT) Then startlevel=(startlevel-1) Mod 36
				If KeyDown(KEY_RIGHT) Then startlevel=(startlevel+1) Mod 36
			End If
			
		End If
		
		Text("LEVEL="+startlevel,0,460)
		
		
		If exit_intro=1 And CountList(explosion_list)=0 Then	exit_intro=2
	
		For explosion:Texplosion = EachIn explosion_list
			explosion.Update()
			explosion.Draw()
		Next
		Flip
	Wend
	If soundpresent Then BASSMOD_MusicStop()
	If soundpresent Then BASSMOD_MusicFree()
	ClearList (bomb_list)
	ClearList (smoke_list)
	ClearList (explosion_list)

	
	If intro_option>=0 And intro_option<=3 Then
		level.Create(startlevel)							'Create level (level id)
		Tplayer.spawn(0,level.p1x,level.p1y,2,0,0,3,1)		'Create Player 1
		If intro_option>0 Then Tplayer.spawn(1,level.p2x,level.p2y,2,0,0,3,0)		'Create Player 2
		If soundpresent Then
			If startlevel<5 Then level.LoadMusic(1)				'Load music from memory (track)
			If startlevel>=5 And startlevel<10 Then level.LoadMusic(2)				'Load music from memory (track)

		End If
		level.AddEnemies()								'Add enemies to level
		SetClsColor(96,96,160)
		level.Fadein()							'Fade level in
		If soundpresent Then BASSMOD_MusicPlayEx(0,BASS_MUSIC_LOOP,False)	'Start music playing
		While CountList(player_list)>0 And Not KeyDown(27)
			While ((CountList(player_list)>0 And level.door_appear<4) Or CountList(HiScoreTable_list)>0) And Not KeyDown(27)
				level.Play()						'Run through main level play routine
				Flip 1								'Flip screen
				If KeyDown(KEY_F12) Then
					Local p:Tpixmap = GrabPixmap (0,0,640,480)
					SavePixmapPNG(p,"screen1.png",5)
				End If
			Wend
			If CountList(player_list)>0 And Not KeyDown(27) Then
				ClearAllLists()
				level.ScrollToNext()
				level.AddEnemies()
				For player:Tplayer = EachIn player_list	'Process/draw player
					player.InitialPosition()
				Next

			End If
		Wend
		level.Fadeout()								'Fade level out
		ClearAllLists()
		ClearList (player_list)
	End If
	
	If soundpresent Then BASSMOD_SetVolume(oldvol)
	If intro_option=7 Then quitgame=1
Wend
									
If soundpresent Then BASSMOD_SetVolume(oldvol)					'Reset system volume
If soundpresent Then BASSMOD_Free()							'Free BASSMOD memory
SaveScores()
End	

Function PutNumberLeadingZeros:String(num:String,zeros)	'Output string from number with leading 
	num=num[num.Length-zeros..]
	Return num.Replace(" ","0")							'Output string
End Function

Function Text(strg$,x,y)						'Output text using graphics font
	'SetBlend (MASKBLEND)
	Local str_length=strg.Length-1
	Local scr_print
	For scr_print=0 To str_length
		DrawImage font,(scr_print*16)+x,y,strg[scr_print]-32
	Next
End Function

Function SmallText(strg$,x,y)						'Output text (numbers only) using small graphics font
	Local str_length=strg.Length-1
	Local scr_print
	For scr_print=0 To str_length
		DrawImage smallfont,(scr_print*8)+x,y,strg[scr_print]-48
	Next
End Function

Function ShowMenu(x:Int,y:Int,t:String,s:Int,f:Int,sb:Int)
	Local z:Int=0
	Local oz:Int=0
	Local r:Int=0
	Local bombx:Int=0
	While z<t.Length
		While t[z]<>Asc("\")
			z:+1
		Wend
		If s=r Then
			For Local i:Int=0 To (z-oz-1)
				Text t[oz+i..oz+i+1],x-((z-(oz+.5))/2)*16+i*16,y+Rand(-3,3)
			Next
			bombx=x-((z-(oz+.5))/2)*16-8
			If sb Then DrawImage bomb_floor,bombx,y+8,f
		Else
			Text t[oz..z],x-((z-(oz+.5))/2)*16,y
		End If
		z:+1
		oz=z
		y:+24
		r:+1
	Wend
	Return bombx
End Function


Function calctime(t:Int)
	Local tot:Int=0
	For Local z:Int=0 To 8
		avetime[z]=avetime[z+1]
		tot:+avetime[z]
	Next
	avetime[9]=t
	tot:+t
	tot:/10
	Return tot
End Function

Function GetKey( )
	For Local n=1 To 255
		If KeyHit(n) Return n
	Next
	Return 0
End Function

Function SaveSettings()
	Local fileout:TStream=WriteFile("fuzed.cfg")
	WriteLine (fileout,"[FUZED]")
	WriteLine (fileout,"")
	WriteLine (fileout,"#KEY DEFINITIONS#")
	WriteLine (fileout,"[P1LEFT]="+pleft[0])
	WriteLine (fileout,"[P1RIGHT]="+pright[0])
	WriteLine (fileout,"[P1JUMP]="+pjump[0])
	WriteLine (fileout,"[P1FIRE]="+pfire[0])
	WriteLine (fileout,"[P1CYCLE]="+pcycle[0])
	WriteLine (fileout,"[P2LEFT]="+pleft[1])
	WriteLine (fileout,"[P2RIGHT]="+pright[1])
	WriteLine (fileout,"[P2JUMP]="+pjump[1])
	WriteLine (fileout,"[P2FIRE]="+pfire[1])
	WriteLine (fileout,"[P2CYCLE]="+pcycle[1])
	WriteLine (fileout,"[PAUSE]="+pausekey)
	WriteLine (fileout,"")
	WriteLine (fileout,"#GRAPHICS DRIVER#")
	WriteLine (fileout,"#0 = DirectX, 1 = OpenGL#")
	WriteLine (fileout,"[GFXDRIVER]="+GfxMode)
	WriteLine (fileout,"")
	WriteLine (fileout,"#STAR BACKGROUND#")
	WriteLine (fileout,"#0 = off, 1 = on#")
	WriteLine (fileout,"[BACKGROUND]="+background_on)
	WriteLine (fileout,"")
	WriteLine (fileout,"[END]")
	WriteLine (fileout,"")
	
	CloseFile(fileout)

End Function

Function LoadSettings()
	Local filein:TStream=ReadFile("fuzed.cfg")
	Local s:String
	s=ReadLine(filein)
	s.Trim
	While s<>"[END]" And Not Eof(filein)
		If s[0]=91 Then 'Found a [
			Local foundfirstpart:Int=0
			Local foundequals:Int=0
			Local foundsecondpart:Int=0
			Local firstpart:String=""
			Local secondpart:String=""
			Local secondpartvalue:Int=0
			Local m:Int=0
			For Local z:Int=1 To s.Length-1
				If s[z]=93 And m=0 Then m=z 'Find first ]
			Next
			If m>0 Then
				firstpart=s[1..m]	'firstpart = text between [ and ]
				foundfirstpart=1
				For Local z:Int=m To s.length
					If s[z]=61 Then		'Found equals sign
						m=z
						foundequals=1
					End If
				Next
				If foundequals
					secondpart=s[m+1..s.length]	'secondpart = everything after equals sign
					secondpart.Trim()
					If secondpart.length>0 Then foundsecondpart=1
					secondpartvalue=Int secondpart
				End If
				
			End If
			If foundfirstpart And foundsecondpart Then
				Select firstpart
					Case "P1LEFT"
						If secondpartvalue>0 Then pleft[0]=secondpartvalue
					Case "P1RIGHT"
						If secondpartvalue>0 Then pright[0]=secondpartvalue
					Case "P1JUMP"
						If secondpartvalue>0 Then pjump[0]=secondpartvalue
					Case "P1FIRE"
						If secondpartvalue>0 Then pfire[0]=secondpartvalue
					Case "P1CYCLE"
						If secondpartvalue>0 Then pcycle[0]=secondpartvalue
					Case "P2LEFT"
						If secondpartvalue>0 Then pleft[1]=secondpartvalue
					Case "P2RIGHT"
						If secondpartvalue>0 Then pright[1]=secondpartvalue
					Case "P2JUMP"
						If secondpartvalue>0 Then pjump[1]=secondpartvalue
					Case "P2FIRE"
						If secondpartvalue>0 Then pfire[1]=secondpartvalue
					Case "P2CYCLE"
						If secondpartvalue>0 Then pcycle[1]=secondpartvalue
					Case "PAUSE"
						If secondpartvalue>0 Then pausekey=secondpartvalue
					Case "GFXDRIVER"
						If secondpartvalue>=0 And secondpartvalue<=1 Then GfxMode=secondpartvalue
					Case "BACKGROUND"
						If secondpartvalue>=0 And secondpartvalue<=1 Then background_on=secondpartvalue

					


				End Select
			End If
		End If
		s=ReadLine(filein)
		s.Trim()
	Wend
	
	CloseFile(filein)
End Function

Function SetGfxMode()
	'Graphics (640,480,32,60|SOFTSYNC)

	'SetGraphicsDriver GLMax2DDriver()
	If GfxMode=0 Then SetGraphicsDriver D3D7Max2DDriver() Else SetGraphicsDriver GLMax2DDriver()			'Select Graphics Driver

	'**** Set Graphics Mode ****
	Rem
	If GraphicsModeExists(640,480,32) Then
		If GraphicsModeExists(640,480,32,60) Then
			Graphics 640,480,32,60|HARDSYNC
		Else
			Graphics 640,480,32,60|SOFTSYNC
		End If
	Else
		If GraphicsModeExists(640,480,16) Then
			If GraphicsModeExists(640,480,16,60) Then
				Graphics 640,480,16,60|HARDSYNC
			Else
				Graphics 640,480,16,60|SOFTSYNC
			End If
		Else
			Notify "Could not initialise graphics mode",True
			End
		End If
	End If
	End Rem

	If GraphicsModeExists(640,480,32) Then
		Graphics 640,480,32,60
		'Graphics 640,480,0,60
	Else
		If GraphicsModeExists(640,480,16) Then
			Graphics 640,480,16,60
		Else
			Notify "Could not initialise graphics mode",True
			End
		End If
	End If


	HideMouse
End Function

Function clearkeys()
	For Local z:Int=0 To 1
		pleft[z]=0
		pright[z]=0
		pjump[z]=0
		pfire[z]=0
		pcycle[z]=0
	Next
	pausekey=0
End Function


Function checkkeyclash(kcode:Int)
	For Local z:Int=0 To 1
		If pleft[z]=kcode Then Return ((z*5)+1)
		If pright[z]=kcode Then Return ((z*5)+2)
		If pjump[z]=kcode Then Return ((z*5)+3)
		If pfire[z]=kcode Then Return ((z*5)+4)
		If pcycle[z]=kcode Then Return ((z*5)+5)
	Next
	If pausekey=kcode Then Return 11
	Return 0
End Function


Function ClearAllLists()
	ClearList (bomb_list)
	ClearList (mine_list)
	ClearList (dynamite_list)
	ClearList (wheelie_list)
	ClearList (grog_list)
	ClearList (slinky_list)
	ClearList (gems_list)
	ClearList (coins_list)
	ClearList (pickedup16_list)
	ClearList (pickedup32_list)
	ClearList (reward_list)
	ClearList (explosion_list)
	ClearList (smoke_list)
	ClearList (pickup_list)
End Function

Function SnaptoGrid16:Int(v:Int)
	Return ((v Shr 4) Shl 4)
End Function

Function CheckScore(s:Int)
	Local z:Byte=0
	Local beat:Byte=0
	
	While z<100 And beat=0
		If s>hiscore[z] Then beat=1
		If beat=0 Then z:+1
	Wend
	
	If beat Then
		For Local i:Int=99 To z Step-1
			hiscore[i]=hiscore[i-1]
			hiname[i]=hiname[i-1]
			hinew[i]=hinew[i-1]
		Next
		Return z
	Else
		Return 10
	End If
	
End Function


Function SaveScores()
	Local f:TStream=WriteFile("Fuzed.hi")
	For Local z:Int=0 To 99
		WriteLine(f,hiname[z])
		WriteInt(f,hiscore[z])
	Next
	CloseFile (f)
End Function

Function LoadScores()
	Local f:TStream=ReadFile("Fuzed.hi")
	For Local z:Int=0 To 99
		hiname[z]=ReadLine(f)
		hiscore[z]=ReadInt(f)
	Next
	CloseFile (f)
End Function