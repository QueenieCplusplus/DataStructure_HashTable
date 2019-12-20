// 2019, 12/20, pm 4:45, by Queenie Chen
//Symbol Table
// Objects: 
// a set of name-attrs pairs,
// where the name is unique

template<class Name, Attrs>
class HashTable
{
    public:
        HashTable(int size = defaultsize);

        bool isInHere(Name name);

        Attrs *find(Name name);
        // if name is in HashTable, then return a * to corresponding attribute; else return 0

        void insertPair(Name name, Attrs at);
        // if the name already exists in HashTable, then replace its attrs (like update)
        // else, insert the pair into HashTable

        void deletePair(Name name);
        // if the name indeed exists in HashTable, then delete the pair of all them.

};