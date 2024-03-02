#include <bits/stdc++.h>
using namespace std;

using point = pair<double, double>;

vector<array<point, 4>> curves;

void bezier(point p0, point p1, point p2, point p3) {
  curves.push_back({p0, p1, p2, p3});
}

string pts(point p) {
  stringstream ss;
  ss << "\\left(" << p.first << ',' << -p.second << "\\right)";
  return ss.str();
}

void line(point p1, point p2) {
  auto [x1, y1] = p1;
  auto [x2, y2] = p2;
  double dx = x2 - x1, dy = y2 - y1;
  bezier(p1, {x1 + dx / 3, y1 + dy / 3}, {x2 - dx / 3, y2 - dy / 3}, p2);
}

void process(string path) {
  bool verbose = true;
  cout << "Processing" << endl;
  cout << path << endl;
  for (char& c : path) c = string("\r\n\t, ").find(c) != string::npos ? ' ' : c;
  cout << "Fixed:" << endl;
  cout << path << endl;
  cout << endl;
  stringstream path_stream(path);

  char mode = 'M';
  double posX = 0, posY = 0;
  double startX = 0, startY = 0;
  double prevCtrlX = 0, prevCtrlY = 0;
  while (path_stream.peek() != EOF) {
    char peeked = path_stream.peek();
    cout << "peek:" << peeked << endl;
    if (peeked == ' ') {
      path_stream.ignore();
      continue;
    }
    if ((peeked >= 'a' && peeked <= 'z') || (peeked >= 'A' && peeked <= 'Z')) {
      path_stream.ignore();
      if (string("MmLlCcSsZz").find(peeked) == string::npos) {
        cout << "unknown modifier: " << peeked << endl;
        exit(-1);
      }
      if (peeked == 'Z' || peeked == 'z') {
        line({posX, posY}, {startX, startY});
        cout << "Finished Path" << endl;
        continue;
      }

      mode = peeked;
      cout << "switched mode to " << mode << endl;
      continue;
    }
    if (mode == 'M' || mode == 'm') {
      double x, y;
      path_stream >> x >> y;
      if (verbose) cout << x << ' ' << y << endl;
      if (mode == 'm') x += posX, y += posY;

      posX = x, posY = y;
      startX = posX, startY = posY;
    }
    else if (mode == 'L' || mode == 'l') {
      double x, y;
      path_stream >> x >> y;
      if (verbose) cout << x << ' ' << y << endl;
      if (mode == 'l') x += posX, y += posY;

      line({posX, posY}, {x, y});
      posX = x, posY = y;
    }
    else if (mode == 'C' || mode == 'c') {
      double x1, y1, x2, y2, x, y;
      path_stream >> x1 >> y1 >> x2 >> y2 >> x >> y;
      if (verbose) cout << x1 << ' ' << y1 << ' ' << x2 << ' ' << y2 << ' ' << x << ' ' << y << endl;
      if (mode == 'c') {
        x1 += posX;
        y1 += posY;
        x2 += posX;
        y2 += posY;
        x += posX;
        y += posY;
      }

      bezier({posX, posY}, {x1, y1}, {x2, y2}, {x, y});

      prevCtrlX = x2;
      prevCtrlY = y2;

      posX = x;
      posY = y;
    }
    else if (mode == 'S' || mode == 's') {
      double x2, y2, x, y;
      path_stream >> x2 >> y2 >> x >> y;
      if (verbose) cout << x2 << ' ' << y2 << ' ' << x << ' ' << y << endl;
      if (mode == 's') {
        x2 += posX;
        y2 += posY;
        x += posX;
        y += posY;
      }

      bezier({posX, posY}, {2 * posX - prevCtrlX, 2 * posY - prevCtrlY}, {x2, y2}, {x, y});

      prevCtrlX = x2;
      prevCtrlY = y2;

      posX = x;
      posY = y;
    }
  }
}

int main(const int argc, const char **argv) {
  basic_regex regex("\\sd=\"([\\s\\S]*?)\"");

  string filename = "a.svg";
  if (argc >= 2) filename = argv[1];

  fstream svg_stream(filename);
  stringstream buffer;
  buffer << svg_stream.rdbuf();
  string svg_data(buffer.str());

  cout << svg_data << endl;

  sregex_iterator iterator(svg_data.begin(), svg_data.end(), regex);
  cout << "Bruh" << endl;
  sregex_iterator end;

  while (iterator != end) {
    smatch match = *iterator;
    cout << match.str() << endl;
    process(match[1]);
    iterator++;
  }

  cout << "P_1=\\left[";
  bool first = true;
  for (auto& curve : curves) {
    if (!first) cout << ',';
    first = false;
    cout << pts(curve[0]);
  }
  cout << "\\right]" << endl;
  cout << "P_2=\\left[";
  first = true;
  for (auto& curve : curves) {
    if (!first) cout << ',';
    first = false;
    cout << pts(curve[1]);
  }
  cout << "\\right]" << endl;
  cout << "P_3=\\left[";
  first = true;
  for (auto& curve : curves) {
    if (!first) cout << ',';
    first = false;
    cout << pts(curve[2]);
  }
  cout << "\\right]" << endl;
  cout << "P_4=\\left[";
  first = true;
  for (auto& curve : curves) {
    if (!first) cout << ',';
    first = false;
    cout << pts(curve[3]);
  }
  cout << "\\right]" << endl;
  cout << "n=\\operatorname{length}\\left(P_{1}\\right)" << endl;
  cout << "c\\left(i,t\\right)=\\left(1-t\\right)^{3}P_{1}\\left[i\\right]+3\\left(1-t\\right)^{2}tP_{2}\\left[i\\right]+3\\left(1-t\\right)t^{2}P_{3}\\left[i\\right]+t^{3}P_{4}\\left[i\\right]" << endl;
  cout << "c\\left(\\operatorname{floor}\\left(nt\\right)+1,\\operatorname{mod}\\left(nt,1\\right)\\right)" << endl;
}
