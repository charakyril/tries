#include <iostream>
#include <string>
#include <vector>
#include <cctype>

using namespace std;

 
    /* ============================================================
       BASE CLASS: Sequence
       ============================================================ */
    class Sequence {
    protected:
        string data;
        static int count;

    public:
        Sequence(const string& d = "") : data(d) {
            ++count;
            cout << "Sequence created. Active sequences: " << count << "\n";
        }

        ~Sequence() {
            --count;
            cout << "Sequence destroyed. Active sequences: " << count << "\n";
        }

        size_t length() const { return data.size(); }
        void describe() const { cout << "Generic sequence: " << data << "\n"; }
        static int getCount() { return count; }
        const string& getData() const { return data; }
    };
    int Sequence::count = 0;

    /* ============================================================
       DNASequence
       ============================================================ */
    class DNASequence : public Sequence {
    public:
        DNASequence(const string& d) : Sequence(d) { cout << "DNASequence created\n"; }
        ~DNASequence() { cout << "DNASequence destroyed\n"; }
        void describe() const { cout << "DNA sequence: " << data << "\n"; }
        static bool isValidBase(char c) { char u = toupper(static_cast<unsigned char>(c)); return u=='A'||u=='C'||u=='G'||u=='T'; }
        bool isValid() const { for (char c : data) if (!isValidBase(c)) return false; return true; }
    };

    /* ============================================================
       RNASequence
       ============================================================ */
    class RNASequence : public Sequence {
    public:
        RNASequence(const string& d) : Sequence(d) { cout << "RNASequence created\n"; }
        ~RNASequence() { cout << "RNASequence destroyed\n"; }
        void describe() const { cout << "RNA sequence: " << data << "\n"; }
        static bool isValidBase(char c) { char u = toupper(static_cast<unsigned char>(c)); return u=='A'||u=='C'||u=='G'||u=='U'; }
        bool isValid() const { for (char c : data) if (!isValidBase(c)) return false; return true; }
    };

    /* ============================================================
       ProteinSequence
       ============================================================ */
    class ProteinSequence : public Sequence {
    public:
        ProteinSequence(const string& d) : Sequence(d) { cout << "ProteinSequence created\n"; }
        ~ProteinSequence() { cout << "ProteinSequence destroyed\n"; }
        void describe() const { cout << "Protein sequence: " << data << "\n"; }
        static bool isValidAA(char c) { const string valid = "ACDEFGHIKLMNPQRSTVWY"; char u = toupper(static_cast<unsigned char>(c)); return valid.find(u) != string::npos; }
        bool isValid() const { for (char c : data) if (!isValidAA(c)) return false; return true; }
    };

    /* ============================================================
       Isoform: contains an RNASequence
       ============================================================ */
    class Isoform {
    private:
        string id;
        string name;
        RNASequence seq;
    public:
        Isoform(const string& i, const string& n, const string& rna) : id(i), name(n), seq(rna) {
            cout << "Isoform created: " << name << "\n";
            if (!seq.isValid()) cout << "Warning: invalid RNA bases in " << name << "\n";
        }
        ~Isoform() { cout << "Isoform destroyed: " << name << "\n"; }
        void describe() const { cout << "Isoform " << id << " (" << name << ")\n"; seq.describe(); cout << "Length: " << seq.length() << " bases\n"; }
    };

    /* ============================================================
       Gene: contains vector<Isoform>
       ============================================================ */
    class Gene {
    private:
        string id;
        string name;
        string chrom;
        int start;
        int end;
        char strand;
        vector<Isoform> isoforms;
    public:
        Gene(const string& gid, const string& gname, const string& chr, int s, int e, char st) : id(gid), name(gname), chrom(chr), start(s), end(e), strand(st) { cout << "Gene created: " << name << "\n"; }
        ~Gene() { cout << "Gene destroyed: " << name << "\n"; }
        void addIsoform(const Isoform& iso) { isoforms.push_back(iso); }
        void createIsoform(const string& i, const string& n, const string& rna) { isoforms.emplace_back(i,n,rna); }
        void describe() const { cout << "Gene " << id << " (" << name << ") on " << chrom << ":" << start << "-" << end << " (" << strand << " strand)\n"; cout << "Active sequences: " << Sequence::getCount() << "\n"; if (isoforms.empty()) { cout << "No isoforms\n"; return; } cout << "Isoforms:\n"; for (const auto& iso : isoforms) iso.describe(); }
    };

    /* ============================================================
       MAIN
       ============================================================ */
    int main() {
        cout << "--- Testing standalone sequences ---\n";
        DNASequence dna("ACGTACGT"); dna.describe(); cout << "Valid? " << (dna.isValid() ? "Yes" : "No") << "\n";
        RNASequence rna("AUGCCAGGAU"); rna.describe(); cout << "Valid? " << (rna.isValid() ? "Yes" : "No") << "\n";
        ProteinSequence prot("MTAKPL"); prot.describe(); cout << "Valid? " << (prot.isValid() ? "Yes" : "No") << "\n";
        cout << "Active sequences now: " << Sequence::getCount() << "\n\n";

        cout << "--- Gene with Isoforms ---\n";
        Gene g("ENSG000001", "TP53", "chr17", 7668402, 7687550, '-');
        g.createIsoform("ENST0001", "TP53-201", "AUGGCCAUGGCGCCC");
        g.createIsoform("ENST0002", "TP53-202", "AUGCCUGAUGCUGUAG");
        Isoform extra("ENST0003", "TP53-203", "AUGGCUAUGGCUUU");
        g.addIsoform(extra);
        cout << "\nCalling g.describe():\n";
        g.describe();
        cout << "\n--- End of main ---\n";
        return 0;
    }
 
