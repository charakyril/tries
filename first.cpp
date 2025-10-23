#include <iostream> // include the iostream header to use std::cout and std::endl
#include <string> // include the string header for std::string type

// Class that represents a protein with id, name and sequence
class Protein { // begin definition of the Protein class
private: // start private section: members not directly accessible from outside
    std::string id_; // private member to store the protein identifier (e.g., "ENSP1234")
    std::string name_; // private member to store the protein name (e.g., "PTEN")
    std::string sequence_; // private member to store the amino acid sequence

public: // start public section: constructors, setters, getters, and methods accessible from outside
    // Constructors
    Protein() = default; // default constructor (compiler-generated implementation)
    Protein(const std::string& id, const std::string& name, const std::string& sequence) // parameterized constructor
        : id_(id), name_(name), sequence_(sequence) {} // initialize members using member initializer list

    // Setters
    void setId(const std::string& id) { id_ = id; } // set the id_ member to the provided id
    void setName(const std::string& name) { name_ = name; } // set the name_ member to the provided name
    void setSequence(const std::string& sequence) { sequence_ = sequence; } // set the sequence_ member

    // Getters
    const std::string& getId() const { return id_; } // return a const reference to id_ (no modification)
    const std::string& getName() const { return name_; } // return a const reference to name_
    const std::string& getSequence() const { return sequence_; } // return a const reference to sequence_

    // describe method: prints "name sequence"
    void describe() const { // describe is a const method (doesn't modify the object)
        std::cout << name_ << " " << sequence_ << std::endl; // print the protein name and sequence separated by a space
    } // end of describe method
}; // end of Protein class

// Class that represents a gene with id, name, chromosomal position and strand
class Gene { // begin definition of the Gene class
private: // private section for internal data
    std::string id_; // private member to store gene identifier (e.g., "ENSG1234")
    std::string name_; // private member to store gene name (e.g., "BRCA1")
    std::string chrom_; // private member to store chromosome (e.g., "chr17")
    long long start_ = 0; // private member to store start coordinate (using long long for large genomic positions)
    long long end_ = 0; // private member to store end coordinate
    char strand_ = '+'; // private member to store strand ('+' or '-') with default '+'

public: // public section for constructors and accessors
    // Constructors
    Gene() = default; // default constructor (compiler-provided)
    Gene(const std::string& id, const std::string& name, const std::string& chrom, // parameterized constructor
         long long start, long long end, char strand)
        : id_(id), name_(name), chrom_(chrom), start_(start), end_(end), strand_(strand) {} // initialize members

    // Setters
    void setId(const std::string& id) { id_ = id; } // set id_
    void setName(const std::string& name) { name_ = name; } // set name_
    void setChrom(const std::string& chrom) { chrom_ = chrom; } // set chrom_
    void setStart(long long start) { start_ = start; } // set start_
    void setEnd(long long end) { end_ = end; } // set end_
    void setStrand(char strand) { strand_ = strand; } // set strand_

    // Getters
    const std::string& getId() const { return id_; } // return id_ by const reference
    const std::string& getName() const { return name_; } // return name_ by const reference
    const std::string& getChrom() const { return chrom_; } // return chrom_ by const reference
    long long getStart() const { return start_; } // return start_ value
    long long getEnd() const { return end_; } // return end_ value
    char getStrand() const { return strand_; } // return strand_ value

    // describe method: prints "Gene NAME (chrom:start-end, strand)."
    void describe() const { // describe is const (no modification)
        std::cout << "Gene " << name_ // print "Gene " followed by the gene name
                  << " (" << chrom_ << ":" << start_ << "-" << end_ << ", " << strand_ << ")." // print position and strand in the requested format
                  << std::endl; // end the line
    } // end of describe
}; // end of Gene class

int main() { // main function: program entry point
    // Example: create a Protein with id, name and sequence
    Protein p1("ENSP1234", "PTEN", "MTAIIKEIVSRNKRRYQEDGFDLDLTYIYPNIIAMGFPA"); // construct a Protein object with given values
    p1.describe(); // call describe on p1 -> prints: PTEN MTAIIKEIVSRNKRRYQEDGFDLDLTYIYPNIIAMGFPA

    // Example: using setters (demonstrates the user-provided line)
    Protein p2; // default-construct a Protein object
    p2.setName("PTEN"); // set the name to "PTEN"
    p2.setSequence("MTAIIKEIVSRNKRRYQEDGFDLDLTYIYPNIIAMGFPA"); // set the sequence to the provided sequence
    p2.describe(); // call describe on p2 -> prints the same name and sequence

    // Example Gene
    Gene g("ENSG1234", "BRCA1", "chr17", 43044295, 43170245, '+'); // construct a Gene object with id, name, chromosome, start, end, strand
    g.describe(); // call describe on g -> prints: Gene BRCA1 (chr17:43044295-43170245, +).

    return 0; // return 0 to indicate successful execution
} // end of main
