#include <iostream>
#include <vector>
#include <numeric>
#include <set>
#include <fstream>
#include <map>
#include <algorithm>

using namespace std;

const int M = 1000006;
bool odw[M]; // visited
bool matchedExperts[M];
int matchedFeatures[M];
vector <int> G[M];
map <int, int> Map;
vector <int> whoInWhich[M]; // for each project list of experts in it

							// turbo matching
bool DFS(int expert) {
	odw[expert] = true;

	for (int i = G[expert].size() - 1; i >= 0; i--) {
		int feature = G[expert][i];//neighbour

		if (matchedFeatures[feature] == -1 || (!odw[matchedFeatures[feature]] && DFS(matchedFeatures[feature]))) {
			matchedFeatures[feature] = expert;
			matchedExperts[expert] = true;
			return true;
		}
	}

	return false;
}

// returns the number of used experts
int UsedExperts(int n, vector <pair <int, int> > pairs) {
	for (int i = 0; i<pairs.size(); i++) {
		G[pairs[i].first].push_back(pairs[i].second);
	}

	bool git = 1;

	while (git == 1) {
		git = 0;

		for (int i = 0; i<n; i++)
			odw[i] = false;

		for (int i = 0; i<n; i++) {
			if (!matchedExperts[i] && !odw[i] && DFS(i))
				git = 1;
		}
	}

	int ExpertsUsed = 0;
	for (int i = 0; i < n; i++)
		if (matchedExperts[i]) {
			ExpertsUsed++;
			matchedExperts[i] = false;
		}

	return ExpertsUsed;
}

int maxProjects(int n, int m, int E, int F, int P, vector <vector<int>> projects, vector<vector<int>> skills) {
	set < pair <int, vector<int> > > S;
	set < pair <int, vector<int> > >::iterator it;

	vector <int> v;

	for (int i = 0; i<F; i++)
		v.push_back(0);

	S.insert(make_pair(0, v));

	for (int i = 0; i<P; i++) {
		it = S.end();
		it--;

		for (; ; it--) {
			vector <int> v = it->second;
			v[i] = 1;
			S.insert(make_pair(it->first + 1, v));

			if (it == S.begin())
				break;
		}
	}

	int result = 0;
	vector<pair <int, int> > pairs;

	for (it = S.begin(); it != S.end(); it++) {
		int index = 0;
		pairs.clear();
		Map.clear();

		// creates pairs expert-feature,which will be used in matching algorithm
		for (int i = 0; i<P; i++)
			if (it->second[i]) // if a project is in a subset
				for (int j = 0; j<F; j++)
					for (int k = 0; k<projects[i][j]; k++) {
						for (int ind = 0; ind<skills[j].size(); ind++) {
							int expertNumber = skills[j][ind];
							int elementNodeIndex = E + index;
							Map[elementNodeIndex] = i; // information in which project is given node
							pairs.push_back(make_pair(expertNumber, elementNodeIndex));
						}
						index++;
					}

		for (int i = E; i<E + index; i++)
			matchedFeatures[i] = -1;
		for (int i = 0; i<n; i++)
			G[i].clear();

		// if there is no missing experts to finish this subset of projects
		//then we can realize this subset
		//index is a number of needed experts
		//cout <<"used expert: "<<UsedExperts(n, m, pairs)<<" index: "<<index<<endl;
		if (UsedExperts(n, pairs) == index) {
			result = it->first;
			for (int i = 0; i<P; i++)
				whoInWhich[i].clear();

			// iteration through all project nodes and collecting information about 
			// which experts are matched to particular projects
			for (int i = E; i<E + index; i++) {
				if (matchedFeatures[i] != -1) {
					int project = Map[i];
					whoInWhich[project].push_back(matchedFeatures[i]);
				}
			}

			for (int i = 0; i<P; i++)
				sort(whoInWhich[i].begin(), whoInWhich[i].end());
			while (it->first == result && it != S.end() && it->first != P)
				it++;
			it--;
			if (result == P)
				break;
		}
	}

	return result;
}

// *** MAIN ***
int main(int argc, char* argv[])
{

	//P- projects, E-experts , F - features
	int n = 0, m, P, E, F, x, projectsNodes, NRofEdges, w;

	fstream data;

	if (!data) {
		cout << "Error while opening the file!!!\n";
		return 1;
	}

	if (argc < 3) {
		cerr << "Usage: name of file parameter: 0 - min number of experts, 1 - max number of projects" << std::endl;
		return 1;
	}

	data.open(argv[1]);

	//w - parameter
	w = strtol(argv[2], NULL, 10);
	data >> P;
	if (data.peek() == ',')
		data.ignore();
	data >> E;
	if (data.peek() == ',')
		data.ignore();
	data >> F;

	//corner case
	if (P == 0) {
		cout << "There are no projects to finish." << endl;
		return 0;
	}

	vector<vector<int>> projects;
	int* projectsFeatures = new int[P];
	int* expertsFeatures = new int[E];
	vector<vector<int>> skills;
	vector<pair <int, int> > pairs;

	for (int i = 0; i < P; i++) {
		projectsFeatures[i] = 0;
	}
	for (int i = 0; i < E; i++) {
		expertsFeatures[i] = 0;
	}

	projectsNodes = 0;
	for (int i = 0; i < P; i++)
	{
		vector<int> row;
		for (int j = 0; j < F; j++) {
			data >> x; // read number of needed expert for one feature
			if (data.peek() == ',')
				data.ignore();
			row.push_back(x);
			projectsFeatures[j] += x;
			projectsNodes += x;
		}
		projects.push_back(row);
	}

	NRofEdges = 0;
	for (int i = 0; i < F; i++) {
		vector<int> row;
		skills.push_back(row);
	}
	for (int i = 0; i<E; i++)
	{
		for (int j = 0; j<F; j++) {
			data >> x; // read if expert know this feature
			if (data.peek() == ',')
				data.ignore();
			expertsFeatures[j] += x;

			if (x) {
				skills[j].push_back(i); // expert number
										// experts which now this skill
			}

			NRofEdges += x * projectsFeatures[j];
		}
	}

	int index = 0;
	for (int i = 0; i<P; i++)
		for (int j = 0; j<F; j++)
			for (int k = 0; k<projects[i][j]; k++) {
				for (int ind = 0; ind<skills[j].size(); ind++) {
					int expertNumber = skills[j][ind];
					int elementNodeIndex = E + index;
					Map[elementNodeIndex] = i; // information in which project is given node
					pairs.push_back(make_pair(expertNumber, elementNodeIndex));
				}
				index++;
			}

	n = E + projectsNodes; //nr of vertices
	m = NRofEdges; // nr of edges

	for (int i = E; i<E + index; i++)
		matchedFeatures[i] = -1;


	if (w == 0) {
		int ExpertsUsed = UsedExperts(E, pairs);

		cout << "Minimal number of missig experts to finish all projects: " << projectsNodes - ExpertsUsed << endl;

		// iteration through all project nodes and collecting information about 
		// which experts are matched to particular projects
		for (int i = E; i<E + index; i++) {
			if (matchedFeatures[i] != -1) {
				int project = Map[i];
				whoInWhich[project].push_back(matchedFeatures[i]);
			}
		}

		for (int i = 0; i<P; i++)
			sort(whoInWhich[i].begin(), whoInWhich[i].end());

		for (int i = 0; i<P; i++) {
			cout << "Project " << i << ": \n";
			if (whoInWhich[i].size() > 0) {
				cout << "Experts: ";
				for (int j = 0; j<whoInWhich[i].size(); j++)
					cout << whoInWhich[i][j] << " ";
			}
			else {
				cout << "Not realised";
			}
			cout << "\n\n";
		}
	}

	if (w == 1) {
		int finishedProjects = maxProjects(E, m, E, F, P, projects, skills);

		cout << "Maximal number of projects that can be finished: " << finishedProjects << endl;

		for (int i = 0; i<P; i++) {
			cout << "Project " << i << ": \n";
			if (whoInWhich[i].size() > 0) {
				cout << "Experts: ";
				for (int j = 0; j<whoInWhich[i].size(); j++)
					cout << whoInWhich[i][j] << " ";
			}
			else {
				cout << "Not realised";
			}
			cout << "\n\n";
		}
	}

	delete[]projectsFeatures;
	delete[]expertsFeatures;

	return 0;
}
