#include <cstdio>
#include <cstdlib>
#include <vector>
#include <utility>
#include <cstring>
#include <algorithm>
#include <climits>

using namespace std;

void draw_lines(vector<vector<int>>&, vector<bool>&, vector<bool>&);
void printv(vector<vector<int>>&);
void printv(vector<bool>&);
void printv(vector<vector<int>>&, vector<bool>&, vector<bool>&);
void printv(vector<int>&);
void printv(pair<int, vector<int>>&);
int printv(pair<int, vector<int>>&, vector<vector<int>>&);
long long printv(pair<int, vector<int>>*, vector<vector<int>>&);
int count_set_bits(vector<bool>&);

int main(int argc, char* argv[]) {
	bool max = false;
	if(argc > 1) {
		if(strcmp(argv[1], "max") == 0) max = true;
	}
	int n, i, j, t, k, s, m;
	scanf("%d", &n);
	vector<vector<int>> orig;
	orig.reserve(n);
	for(i = 0; i < n; ++i) {
		vector<int> v;
		v.reserve(n);
		for(j = 0; j < n; ++j) {
			scanf("%d", &t);
			v.push_back(t);
		}
		orig.push_back(v);
	}
	vector<vector<int>> ws = orig;
	if(max) {
		t = ws[0][0];
		for(auto& v : ws) {
			t = *max_element(v.cbegin(), v.cend());
		}
		for(auto& v : ws) {
			for(auto& f : v) {
				f = t - f;
			}
		}
	}

	vector<bool> rla(n, false), cla(n, false);
	
	// Row reduction
	for(auto& v : ws) {
		t = *min_element(v.cbegin(), v.cend());
		for(auto& p : v) {
			p -= t;
		}
	}

	// Column reduction
	for(i = 0; i < n; ++i) {
		t = ws[0][i];
		for(j = 0; j < n; ++j) {
			if(ws[j][i] < t) t = ws[j][i];
		}
		for(j = 0; j < n; ++j) {
			ws[j][i] -= t;
		}
	}

	// Draw lines
	draw_lines(ws, rla, cla);

	// While number of lines < n
	while(count_set_bits(rla) + count_set_bits(cla) < n) {
		//printv(ws, rla, cla);
		t = INT_MAX;
		for(i = 0; i < n; ++i) {
			for(j = 0; j < n; ++j) {
				if(rla[i] || cla[j]) continue;
				if(ws[i][j] < t) t = ws[i][j];
			}
		}
		//printf("\nt = %d\n", t);
		for(i = 0; i < n; ++i) {
			for(j = 0; j < n; ++j) {
				if(rla[i] ^ cla[j]) continue;
				if(rla[i] && cla[j]) ws[i][j] += t;
				else ws[i][j] -= t;
			}
		}
		fill(rla.begin(), rla.end(), false);
		fill(cla.begin(), cla.end(), false);
		draw_lines(ws, rla, cla);
	}

	printv(ws, rla, cla);

	// Result pair array
	pair<int, vector<int>> res[n];
	for(i = 0; i < n; ++i) {
		vector<int> v;
		for(j = 0; j < n; ++j) {
			if(ws[i][j] == 0) v.push_back(j);
		}
		res[i] = make_pair(i, v);
	}

	vector<int> log(n, 0);
	for(i = 0; i < n; ++i) {
		for_each(res[i].second.cbegin(), res[i].second.cend(), [&log] (int f) {
			++log[f];
		});
	}

	for(i = 0; i < n; ++i) {
		for(auto f : res[i].second) {
			if(log[f] == 1) {
				vector<int> v;
				v.push_back(f);
				for(auto z : res[i].second) {
					if(log[z] > 0) --log[z];
				};
				res[i].second = v;
				break;
			}
		}
		if(res[i].second.size() > 1) {
			for(auto z : res[i].second) {
				if(log[z] != 0) {
					t = z;
					break;
				}
			}
			for(j = i+1; j < n; ++j) {
				vector<int> v;
				for(auto z : res[j].second) {
					if(z == t) {
						if(log[z] > 0) --log[z];
					}
					else {
						v.push_back(z);
					}
				};
				res[j].second = v;
			}
			for_each(res[i].second.cbegin(), res[i].second.cend(), [&log] (int z) {
				if(log[z] > 0) --log[z];
			});
			res[i].second.clear();
			res[i].second.push_back(t);
		}
		log[res[i].second[0]] = 0;
	}


	printv(res, orig);

	printf("\n\nAll done\n");

	return 0;
}

void draw_lines(vector<vector<int>>& ws, vector<bool>& rla, vector<bool>& cla) {
	int h, r, c, s, t, k, a, b;
	// Draw lines on rows
	s = ws.size();
	h = 0;
	for(int i = 0; i < s; ++i) {
		t = 0;
		k = ws[i].size();
		for(int j = 0; j < k; ++j) {
			if(ws[i][j] == 0) ++t;
		}
		if(t > h) {
			h = t;
			r = i;
		}
	}
	rla[r] = true;

	// Draw lines on columns
	s = ws[0].size();
	h = 0;
	k = ws.size();
	for(int i = 0; i < s; ++i) {
		t = 0;
		for(int j = 0; j < k; ++j) {
			if(ws[j][i] == 0) ++t;
		}
		if(t > h) {
			h = t;
			c = i;
		}
	}
	cla[c] = true;

	// Number of rows and columns in the matrix
	r = ws.size();
	c = ws[0].size();
	// For each zero not on line
	// If has more zeros on rows set rla[i] to true else set cla[i] to true
	for(int i = 0; i < r; ++i) {
		for(int j = 0; j < c; ++j) {
			if(ws[i][j] != 0) continue;
			if(rla[i] || cla[j]) continue;
			t = a = b = 0;
			// Row zero count
			for(int m = 0; m < r; ++m) {
				if(ws[i][m] == 0 && !(rla[i] || cla[m])) ++a;
			}
			// Column zero count
			for(int m = 0; m < r; ++m) {
				if(ws[m][j] == 0 && !(rla[m] || cla[j])) ++b;
			}
			// Set appropriate line array to zero
			if(a > b) {
				rla[i] = true;
			}
			else {
				cla[j] = true;
			}
		}
	}

	return;
}

void printv(vector<vector<int>>& mat) {
	int r = mat.size(), c = 0, t;
	for(int i = 0; i < r; ++i) {
		for(int j = 0; j < c; ++j) {
			t = mat[i][j];
			if(j == c-1) printf("%d\n", t);
			else printf("%d, ", t);
		}
	}
	return;
}

void printv(vector<bool>& la) {
	int n = la.size(), t;
	for(int i = 0; i < n; ++i) {
		t = la[i]? 1 : 0;
		printf("%d, ", t);
	}
	printf("\n");
	return;
}

void printv(vector<vector<int>>& mat, vector<bool>& rla, vector<bool>& cla) {
	int r = rla.size(), c = cla.size(), t, h, i, j;
	printf("\n ");
	for(i = 0; i < c; ++i) printf("%4d", cla[i]? 1 : 0);
	printf("\n");
	for(i = 0; i < r; ++i) {
		printf("%d", rla[i]? 1: 0);
		for(j = 0; j < c; ++j) printf("%4d", mat[i][j]);
		printf("\n");
	}
	return;
}

void printv(vector<int>& v) {
	int n = v.size();
	printf("\n");
	for(int i = 0; i < n; ++i) {
		printf("%d%s", v[i], i == n-1? "\n":", ");
	}
	return;
}

void printv(pair<int, vector<int>>& p) {
	int n = p.second.size();
	printf("\n");
	printf("%d => ", p.first);
	for(int i = 0; i < n; ++i) {
		printf("%d%s", p.second[i], i == n-1? "\n":", ");
	}
	return;
}

int printv(pair<int, vector<int>>& p, vector<vector<int>>& orig) {
	int n = p.second.size(), t;
	printf("\n");
	printf("%c => ", p.first+65);
	for(int i = 0; i < n; ++i) {
		t = orig[p.first][p.second[i]];
		printf("%d%s", t, i == n-1? "\n":", ");
	}
	return t;
}

long long printv(pair<int, vector<int>> *p, vector<vector<int>>& orig) {
	int n = orig.size();
	long long total = 0;
	for(int i = 0; i < n; ++i) {
		total += printv(p[i], orig);
	}
	printf("\nTotal => %lld\n", total);
	return total;
}

int count_set_bits(vector<bool>& v) {
	int s = 0;
	for(auto t = v.cbegin(); t != v.cend(); ++t) {
		if(*t) ++s;
	}
	return s;
}
