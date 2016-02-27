treeType = TreeType(Vector(UChar_t))

fill = r"""
TTree *t = new TTree("t", "");
std::vector<unsigned char> x;
t->Branch("x", &x);
x = {0, 1, 2};
t->Fill();
x = {10, 11, 12};
t->Fill();
x = {100, 101, 102};
t->Fill();
x = {20, 21, 22};
t->Fill();
"""

schema = {"type": "record",
          "name": "t",
          "fields": [{"name": "x", "type": {"type": "array", "items": "int"}}]}

json = [{"x": [0, 1, 2]},
        {"x": [10, 11, 12]},
        {"x": [100, 101, 102]},
        {"x": [20, 21, 22]}]
