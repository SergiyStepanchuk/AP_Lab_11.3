// Lab_11_3
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <string>
#include <fstream>
#include <algorithm>
using namespace std;

// Console 

namespace cons {

	COORD GetBufferSize() {
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbInfo;
		GetConsoleScreenBufferInfo(handle, &csbInfo);
		return { csbInfo.srWindow.Right - csbInfo.srWindow.Left ,
				csbInfo.srWindow.Bottom - csbInfo.srWindow.Top };
	}

	const COORD size = GetBufferSize();

	void clear() {
		system("cls");
	}

	void gotoxy(const COORD pos) {
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleCursorPosition(handle, pos);
	}

	COORD getxy() {
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(handle, &csbi))
			return { -1, -1 };
		return csbi.dwCursorPosition;
	}

	void clearto(const COORD pos) {
		COORD current_pos = getxy();
		while (current_pos.Y >= pos.Y)
		{
			if (current_pos.Y > pos.Y) {
				gotoxy({ 0, current_pos.Y });
				for (int i = 0; i < size.X; i++)
					cout << ' ';
			}
			else if (current_pos.Y == pos.Y) {
				gotoxy({ pos.X, current_pos.Y });
				for (int i = 0; i <= size.X - pos.X; i++)
					cout << ' ';
			}

			current_pos.Y--;
		}
		gotoxy(pos);
	}

	void change_cusor_visibility(const bool& rst) {
		static HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_CURSOR_INFO structCursorInfo;
		GetConsoleCursorInfo(handle, &structCursorInfo);
		structCursorInfo.bVisible = rst;
		SetConsoleCursorInfo(handle, &structCursorInfo);
	}

	template <typename T>
	T input(bool (*check)(T& temp, char* err), const bool& rom, const char* text, ...) {
		COORD start[2] = { getxy() };
		char out[256] = { 0 }, err[256] = { 0 };
		T temp;

		va_list  args;
		va_start(args, text);
		vsprintf_s(out, 255, text, args);
		va_end(args);
		cout << out;
		start[1] = getxy();
		if (check == nullptr)
			check = [](T& temp, char* err) -> bool { return !cin.fail(); };
		do {
			if (cin.fail()) {
				cin.clear();
				cin.ignore(INT_MAX, '\n');
			}
			change_cusor_visibility(false);
			if (err[0] != '\0') {
				clearto(start[0]);
				cout << err << endl;
				err[0] = '\0';
				cout << out;
				start[1] = getxy();
			}
			else clearto(start[1]);
			change_cusor_visibility(true);
			cin >> temp;
		} while (!check(temp, err));
		if (rom)
			clearto(start[0]);
		return temp;
	}
}

enum StudentProfession : unsigned short {
	StudentProfession_ComputerScience = 0, // Комп’ютерні науки // програмування
	StudentProfession_Informatics, // Інформатика // чисельних методів
	StudentProfession_MathematicsEconomics, // Математика та економіка // педагогіки
	StudentProfession_PhysicsComputerScience, //  Фізика та інформатика // педагогіки
	StudentProfession_WorkTraining // Трудове навчання // педагогіки
};

struct Student {
	char lastname[120]; // прізвище
	unsigned short course; // курс
	StudentProfession profession; // спеціальність

	unsigned short mark_physics; // фізика
	unsigned short mark_math; // математика
	union
	{
		unsigned short programming; // програмування
		unsigned short numerical_methods; // чисельних методів
		unsigned short pedagogy; // педагогіки
	};
};

void writeStudent(fstream& f, const int& index, const Student& st) {
	f.clear();
	f.seekp(index * sizeof(Student));
	f.write((char*)&st, sizeof(Student));
}

void readStudent(fstream& f, const int& index, Student &st) {
	f.clear();
	f.seekg(index * sizeof(Student));
	f.read((char*)&st, sizeof(Student));
}

void swapStudents(fstream& f, const int& i1, const int& i2) {
	Student st1, st2;
	readStudent(f, i1, st1);
	readStudent(f, i2, st2);
	writeStudent(f, i1, st2);
	writeStudent(f, i2, st1);
}

void studentFactory(fstream &f, const int& count) {
	static auto check_course = [](unsigned short& temp, char* err) -> bool {
		if (cin.fail() ||
			temp < 1 ||
			temp > 10)
		{
			sprintf_s(err, 255, "Incorrect cource(%hu)", temp);
			return false;
		}
		return true;
	};

	static auto check_profession = [](unsigned short& temp, char* err) -> bool {
		if (cin.fail() ||
			temp > 4)
		{
			sprintf_s(err, 255, "Incorrect profession(%hu)", temp);
			return false;
		}
		return true;
	};

	static auto check_mark = [](unsigned short& temp, char* err) -> bool {
		if (cin.fail() ||
			temp > 5)
		{
			sprintf_s(err, 255, "Incorrect mark(%hu)", temp);
			return false;
		}
		return true;
	};
	int prof;
	COORD pos = cons::getxy();
	Student tmp;
	for (int i = 0; i < count; i++)
	{
		cout << "Student № " << i + 1 << ":" << endl;
		strcpy_s(tmp.lastname, 119, cons::input<string>(nullptr, false, "Lastname: ").c_str());
		tmp.course = cons::input<unsigned short>(check_course, false, "Course  : ");
		cout << "Professions:\n0 - Computer science\n1 - Informatics\n2 - Mathematics and economics\n3 - Physics and computer science\n4 - Work training\n";
		tmp.profession = (StudentProfession)cons::input<unsigned short>(check_profession, false, "Select profession: ");
		switch (tmp.profession)
		{
		case StudentProfession_ComputerScience:
			tmp.programming = cons::input<unsigned short>(check_mark, false, "Mark of programming: ");
			break;
		case StudentProfession_Informatics:
			tmp.programming = cons::input<unsigned short>(check_mark, false, "Mark of numerical methods: ");
			break;
		case StudentProfession_MathematicsEconomics:
		case StudentProfession_PhysicsComputerScience:
		case StudentProfession_WorkTraining:
			tmp.programming = cons::input<unsigned short>(check_mark, false, "Mark of pedagogy: ");
		}
		tmp.mark_physics = cons::input<unsigned short>(check_mark, false, "Mark of physics: ");
		tmp.mark_math = cons::input<unsigned short>(check_mark, false, "Mark of math: ");
		f.write((char*)&tmp, sizeof(Student));
		cons::clearto(pos);
	}
}

void drawStudents(fstream &f)
{
	cout << "===================================================================================================================" << endl;
	cout << "| № | Lastname | Course | Profession                  | Physics | Math | Programming | Numerical methods | Pedagogy" << endl;
	cout << "-------------------------------------------------------------------------------------------------------------------" << endl;
	Student tmp;	
	int i = 0;
	f.clear();
	f.seekg(0);
	while (!f.eof()) {
		f.read((char*)&tmp, sizeof(Student));
		if (f.eof())
			break;
		cout << "| " << setw(2) << ++i
			<< "| " << setw(9) << tmp.lastname
			<< "| " << setw(7) << tmp.course;

		switch (tmp.profession)
		{
		case StudentProfession_ComputerScience:
			cout << "| " << setw(28) << "Comp science";
			break;
		case StudentProfession_Informatics:
			cout << "| " << setw(28) << "Informatics";
			break;
		case StudentProfession_MathematicsEconomics:
			cout << "| " << setw(28) << "Math and econ";
			break;
		case StudentProfession_PhysicsComputerScience:
			cout << "| " << setw(28) << "Phys and comp sci";
			break;
		case StudentProfession_WorkTraining:
			cout << "| " << setw(28) << "Work training";
			break;
		}
		cout << "| " << setw(8) << tmp.mark_physics
			<< "| " << setw(5) << tmp.mark_math;

		switch (tmp.profession)
		{
		case StudentProfession_ComputerScience:
			cout << "| " << setw(12) << tmp.programming
				<< "| " << setw(18) << " "
				<< "| " << setw(8) << " ";
			break;
		case StudentProfession_Informatics:
			cout << "| " << setw(12) << " "
				<< "| " << setw(18) << tmp.programming
				<< "| " << setw(8) << " ";
			break;
		case StudentProfession_MathematicsEconomics:
		case StudentProfession_PhysicsComputerScience:
		case StudentProfession_WorkTraining:
			cout << "| " << setw(12) << " "
				<< "| " << setw(18) << " "
				<< "| " << setw(8) << tmp.programming;
		}
		cout << endl;

	}
	cout << "===================================================================================================================" << endl;
}

void indexed_sort(fstream &sourse, fstream &out)
{
	Student tmp[2];
	int i_tmp[2];
	out.clear();
	sourse.clear();
	out.seekp(0);
	sourse.seekg(0);
	int size = 0;
	while (!sourse.eof()) {
		sourse.read((char*)&tmp[0], sizeof(Student));
		if (sourse.eof())
			break;
		out.write((char*)&size, sizeof(int));
		size++;
	}
	sourse.clear();
	sourse.seekg(0);
	for (int i = 0; i < size - 1; ++i)
	{
		for (int j = i; j < size - i - 1; ++j)
		{
			out.seekg(j * sizeof(int));
			out.read((char*)&i_tmp[0], sizeof(int));
			readStudent(sourse, i_tmp[0], tmp[0]);

			out.seekg((j+1) * sizeof(int));
			out.read((char*)&i_tmp[1], sizeof(int));
			readStudent(sourse, i_tmp[1], tmp[1]);

			int avarage1 = (tmp[0].mark_math + tmp[0].mark_physics + tmp[0].programming) / 3;
			int avarage2 = (tmp[1].mark_math + tmp[1].mark_physics + tmp[1].programming) / 3;
			if (avarage1 > avarage2 ||
				(avarage1 == avarage2 && tmp[0].lastname > tmp[1].lastname) ||
				(avarage1 == avarage2 && tmp[0].lastname == tmp[1].lastname && tmp[0].course > tmp[1].course))
			{
				out.seekp(j * sizeof(int));
				out.write((char*)&i_tmp[1], sizeof(int));
				out.seekp((j+1) * sizeof(int));
				out.write((char*)&i_tmp[0], sizeof(int));
			}
		}
	}
}

void sort_students(fstream& f)
{
	Student tmp[2];
	f.clear();
	f.seekg(0, ios::end);
	int size = f.tellg()/sizeof(Student);
	for (int i = 0; i < size - 1; ++i)
	{
		for (int j = i; j < size - i - 1; ++j)
		{
			readStudent(f, j, tmp[0]);
			readStudent(f, j+1, tmp[1]);

			int avarage1 = (tmp[0].mark_math + tmp[0].mark_physics + tmp[0].programming) / 3;
			int avarage2 = (tmp[1].mark_math + tmp[1].mark_physics + tmp[1].programming) / 3;
			if (avarage1 > avarage2 ||
				(avarage1 == avarage2 && tmp[0].lastname < tmp[1].lastname) ||
				(avarage1 == avarage2 && tmp[0].lastname == tmp[1].lastname && tmp[0].course > tmp[1].course))
				swapStudents(f, j, j + 1);
		}
	}
}

void print_indexed_sort(fstream& f, fstream& indexed)
{
	cout << "===================================================================================================================" << endl;
	cout << "| № | Lastname | Course | Profession                  | Physics | Math | Programming | Numerical methods | Pedagogy" << endl;
	cout << "-------------------------------------------------------------------------------------------------------------------" << endl;
	Student tmp;
	int tmp_i = 0;
	indexed.clear();
	indexed.seekg(0);
	while (!indexed.eof()) {
		indexed.read((char*)&tmp_i, sizeof(int));
		if (indexed.eof())
			break;
		readStudent(f, tmp_i, tmp);
		cout << "| " << setw(2) << tmp_i
			<< "| " << setw(9) << tmp.lastname
			<< "| " << setw(7) << tmp.course;

		switch (tmp.profession)
		{
		case StudentProfession_ComputerScience:
			cout << "| " << setw(28) << "Comp science";
			break;
		case StudentProfession_Informatics:
			cout << "| " << setw(28) << "Informatics";
			break;
		case StudentProfession_MathematicsEconomics:
			cout << "| " << setw(28) << "Math and econ";
			break;
		case StudentProfession_PhysicsComputerScience:
			cout << "| " << setw(28) << "Phys and comp sci";
			break;
		case StudentProfession_WorkTraining:
			cout << "| " << setw(28) << "Work training";
			break;
		}
		cout << "| " << setw(8) << tmp.mark_physics
			<< "| " << setw(5) << tmp.mark_math;

		switch (tmp.profession)
		{
		case StudentProfession_ComputerScience:
			cout << "| " << setw(12) << tmp.programming
				<< "| " << setw(18) << " "
				<< "| " << setw(8) << " ";
			break;
		case StudentProfession_Informatics:
			cout << "| " << setw(12) << " "
				<< "| " << setw(18) << tmp.programming
				<< "| " << setw(8) << " ";
			break;
		case StudentProfession_MathematicsEconomics:
		case StudentProfession_PhysicsComputerScience:
		case StudentProfession_WorkTraining:
			cout << "| " << setw(12) << " "
				<< "| " << setw(18) << " "
				<< "| " << setw(8) << tmp.programming;
		}
		cout << endl;

	}
	cout << "===================================================================================================================" << endl;
}

/*void sort_students(Student* st, const int& count) {
	sort(st, st + count, [&](const Student& a1, const Student& a2) -> bool {
		int avarage1 = (a1.mark_math + a1.mark_physics + a1.programming) / 3;
		int avarage2 = (a2.mark_math + a2.mark_physics + a2.programming) / 3;
		if (avarage1 > avarage2 ||
			(avarage1 == avarage2 && a1.lastname < a2.lastname) ||
			(avarage1 == avarage2 && a1.lastname == a2.lastname && a1.course > a2.course))
			return true; // міняємо
		return false;
		}
	);
}*/

/*int	bin_search_student(Student* st, const int& count, const string lastname, const unsigned short& course, const unsigned short& avarage) {
	int L = 0, R = count - 1, m;
	do {
		m = (L + R) / 2;
		if ((st[m].mark_math + st[m].mark_physics + st[m].programming) / 3 < avarage)
			L = m + 1;
		else if ((st[m].mark_math + st[m].mark_physics + st[m].programming) / 3 == avarage) {
			if (st[m].lastname < lastname)
				L = m + 1;
			else if (st[m].lastname == lastname) {
				if (st[m].course < course)
					L = m + 1;
				else if (st[m].course == course)
					return m;
				else R = m - 1;
			}else R = m - 1;
		}else R = m - 1;
	} while (L <= R);
}*/

void main() {
	SetConsoleCP(1251); // встановлення сторінки win-cp1251 в потік вводу
	SetConsoleOutputCP(1251); // встановлення сторінки win-cp1251 в потік виводу
	cout.setf(ios_base::left);

	unsigned int state = 1, count = 3;
	fstream f;
	bool indexed_cout = false;
	char message[256] = { 0 };
	string name_of_file = cons::input<string>(nullptr, false, "Input name of file: ");
	f.open(name_of_file, ios::binary | ios::in | ios::out);
	if (!f.is_open()) {
		f.open(name_of_file, ios::binary | ios::in | ios::out | ios::trunc);
		if (!f.is_open()) {
			cout << "can't open file!" << endl;
			return;
		}
		
		
	}
	else f.seekp(0, ios::end);
	count = cons::input<unsigned int>([](unsigned int& temp, char* err) {
		if (cin.fail() ||
			temp < 0 ||
			temp > 20) {
			sprintf_s(err, 255, "Incorrect count(%d)!", temp);
			return false;
		}
		return true;
		}, true, "Input count of students (1 <= count <= 20): ");
	studentFactory(f, count);

	do {
		cons::clear();
		//cout << bin_search_student(st, count, "abcd max", 2, 3) << endl;
		if (!indexed_cout)
			drawStudents(f);
		else {
			fstream fi(cons::input<string>(nullptr, true, "Input name of file to index sort: "), ios::binary | ios::in | ios::trunc | ios::out);
			if (fi.is_open()) {
				indexed_sort(f, fi);
				print_indexed_sort(f, fi);
				fi.close();
			}
			else cout << "Can't open file for indexed sort!";
			indexed_cout = false;
		}
		if (message[0] != '\0') {
			cout << message << endl;
			message[0] = '\0';
		}
		cout << "[1] Sort by lastname" << endl
			<< "[2] Indexed cout" << endl
			<< "[3] Сout" << endl
			<< "[0] Exit" << endl;
		state = cons::input<unsigned int>(nullptr, false, "Select action: ");
		switch (state)
		{
		case 1:
			sort_students(f);
			sprintf_s(message, 255, "Succesfully sorted by lastname");
			break;
		case 2:
			indexed_cout = true;
			sprintf_s(message, 255, "Succesfully indexed sort");
			break;
		case 3:
			break;
		default:
			sprintf_s(message, 255, "Incorrect action!");
		}
	} while (state > 0);
	f.close();
}