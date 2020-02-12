
#include <iostream>
#include <random>
#include <map>
using namespace std;

enum MAP_DIRECTION{
	NORTH = 0,
	EAST = 1,
	WEST = 2,
	SOUTH = 3,
};

class MapSite{
public:
	virtual void print() = 0;
};

class Room : public MapSite{
protected:
	static int no;
	int roomNo;
	MapSite* map[4] = {};
public:
	Room(){
		roomNo = no++;
	}
	~Room(){
		for(int i = 0; i < 4; i++){
			free(map[i]);
		}
	}
	int getRoomNo(){
		return roomNo;
	}
	MapSite* getAdjRoom(int d){
		return map[d];
	}
	void setAdjRoom(int d, MapSite* m){
		map[d] = m;
	}
	void print();

};

int Room::no = 0;

void Room::print(){
	cout << "Room#" << this->roomNo;
}

class EnchantedRoom : public Room{
private:
	string item;
public:
	string getItem(){
		return item;
	}
	void setItem(string i){
		item = i;
	}
	void print();
};

void EnchantedRoom::print(){
	cout << "EnchantedRoom(" << item << ")#" << this->roomNo;
}

class Door : public MapSite{
protected:
	Room* room1;
	Room* room2;
	bool isOpen;
public:
	MapSite* getRoom1(){
		return room1;
	}
	MapSite* getRoom2(){
		return room2;
	}
	bool getIsOpen(){
		return isOpen;
	}
	void setRoom(Room* r1, Room* r2, bool b){
		room1 = r1;
		room2 = r2;
		isOpen = b;
	}
	bool open(){
		isOpen = true;
		return true;
	}
	bool close(){
		isOpen = false;
		return true;
	}
	void print();
};

void Door::print(){
	cout << "Door(";
	room1->print();
	printf(", ");
	room2->print();
	cout << ")";
}


class SpellDoor : public Door{
private:
	string spell;
public:
	string getSpell(){
		return spell;
	}
	void setSpell(string s){
		spell = s;
	}
	bool open(){
		return isOpen;
	}
	bool open(string s){
		if(spell == s) return Door::open();
		return false;
	}
	void print();

};

void SpellDoor::print(){
	cout << "Door(";
	room1->print();
	printf(", ");
	room2->print();
	printf(", ");
	cout << "Spell: " << spell;
	cout << ")";
}

class Wall : public MapSite{
public:
	void print();

};

void Wall::print(){
	cout << "Wall";
}


class Maze{
private:
	static mt19937 mt;
	map<int, Room*>* rooms;
public:
	static int rand;
	static void initRand(){
		rand = mt();
	}
	static void nextRand(){
		rand = mt();
	}
	Maze(){
		rooms = new map<int, Room*>();
	}
	Room* findRoom(int n){
		return rooms->at(n);
	}
	void addRoom(Room* aRoom){
		rooms->insert(make_pair(aRoom->getRoomNo(), aRoom));
	}
	void print(int roomNo){
		Room* r = this->findRoom(roomNo);
		r->print();
		cout << "(";
		r->getAdjRoom(NORTH)->print();
		cout << ", ";
		r->getAdjRoom(EAST)->print();
		cout << ", ";
		r->getAdjRoom(WEST)->print();
		cout << ", ";
		r->getAdjRoom(SOUTH)->print();
		cout << ")" << endl;
	}

};

int Maze::rand;
mt19937 Maze::mt;


class MazeGame{
public:
	Maze* makeMaze(){
		Maze* m;
		m = new Maze();
		return m;
	}

	virtual Room* makeRoom(){
		Room* r;
		r = new Room();
		return r;
	}

	Wall* makeWall(){
		return new Wall();
	}

	virtual Door* makeDoorFrom(Room* r1, Room* r2){
		Door* d;
		d = new Door();
		d->setRoom(r1, r2, false);
		return d;
	}

	Maze* createMaze(){
		Maze* maze;
		Room* r1, * r2;
		Door* d;
		maze = this->makeMaze();
		r1 = this->makeRoom();
		r2 = this->makeRoom();
		d = this->makeDoorFrom(r1, r2);
		maze->addRoom(r1);
		maze->addRoom(r2);
		r1->setAdjRoom(NORTH, this->makeWall());
		r1->setAdjRoom(SOUTH, this->makeWall());
		r1->setAdjRoom(WEST, this->makeWall());
		r1->setAdjRoom(EAST, d);
		r2->setAdjRoom(NORTH, this->makeWall());
		r2->setAdjRoom(SOUTH, this->makeWall());
		r2->setAdjRoom(EAST, this->makeWall());
		r2->setAdjRoom(WEST, d);
		return maze;
	}
};

class MagicMazeGame: public MazeGame{
private:
	string genItem(){
		Maze::nextRand();
		int rnd = Maze::rand;
		rnd %= 3;
		if(rnd == 0) return "Bomb";
		if(rnd == 1) return "Diamond";
		return "Scroll";
	}
	string genSpell(){
		Maze::nextRand();
		int rnd = Maze::rand;
		rnd %= 3;
		if(rnd == 0) return "Hello World";
		if(rnd == 1) return "Abracadabra";
		return "Iroha";

	}
public:
	EnchantedRoom* makeRoom(){
		EnchantedRoom* r = new EnchantedRoom();
		r->setItem(this->genItem());
		return r;
	}
	SpellDoor* makeDoorFrom(Room* r1, Room* r2){
		SpellDoor* d = new SpellDoor();
		d->setRoom(r1, r2, false);
		d->setSpell(this->genSpell());
		return d;
	}
};

int main(){
	Maze::rand = 0;
	MazeGame* m = new MagicMazeGame();
	Maze* m1 = m->createMaze();
	m1->print(0);
	m1->print(1);

}
