// Simple, compact C++ program with Protein and Gene classes.
// Build: g++ -std=c++17 main.cpp -o main
// Run:   ./main

#include <iostream>
#include <string>

class Protein {
private:
    std::string id_, name_, seq_;
public:
    Protein(const std::string& id, const std::string& name, const std::string& seq)
        : id_(id), name_(name), seq_(seq) {}

    // setters
    void setId(const std::string& v) { id_ = v; }
    void setName(const std::string& v) { name_ = v; }
    void setSequence(const std::string& v) { seq_ = v; }

    // getters
    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& sequence() const { return seq_; }

    void describe() const {
        std::cout << "Protein " << name_ << " (" << seq_ << ")\n";
    }

    std::size_t length() const { return seq_.size(); } // optional
};

class Gene {
private:
    std::string id_, name_, chrom_;
    long long start_ = 0, end_ = 0;
    char strand_ = '+';
public:
    Gene(const std::string& id, const std::string& name,
         const std::string& chrom, long long start, long long end, char strand)
        : id_(id), name_(name), chrom_(chrom), start_(start), end_(end), strand_(strand) {}

    void describe() const {
        std::cout << "Gene " << name_ << " (" << chrom_ << ":" << start_
                  << "-" << end_ << ", " << strand_ << ").\n";
    }
};

int main() {
    // Two proteins (PTEN sequence provided)
    Protein p1("ENSP000001", "PTEN", "MTAIIKEIVSRNKRRYQEDGFDLDLTYIYPNIIAMGFPA");
    Protein p2("ENSP000002", "TP53", "MEEPQSDPSVEPPLSQETFSDLWKLLPEN");

    p1.describe();
    std::cout << "  ID: " << p1.id() << ", length: " << p1.length() << "\n\n";

    p2.describe();
    std::cout << "  ID: " << p2.id() << ", length: " << p2.length() << "\n\n";

    // Two genes
    Gene g1("ENSG00000012048", "BRCA1", "chr17", 43044295, 43170245, '+');
    Gene g2("ENSG00000141510", "TP53", "chr17", 7661779, 7687550, '-');

    g1.describe();
    g2.describe();

    return 0;
}