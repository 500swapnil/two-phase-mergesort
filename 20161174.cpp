#include<bits/stdc++.h>

using namespace std;

string order;
vector<int> sort_order;
int line_size = 0;
vector<int> c_line(100000, 0);
vector<int> b_line(100000, 0);
char *outfile;
priority_queue<pair<vector<string>,int>,vector<pair<vector<string>,int> > > desc;
priority_queue<pair<vector<string>,int>,vector<pair<vector<string>,int> >,greater<pair<vector<string>,int> > > asc;


vector<string> split_string(string a, char delimiter)
{
        istringstream tokenStream(a);
        vector<string> tokens;
        string word;
        while(getline(tokenStream,word,delimiter))
        {
            tokens.push_back(word);
        }
        return tokens;
}

void add_line(int blocknum, int num)
{
        if(b_line[blocknum] < num)
                return;
        string line;
        string filename=("file" + to_string(blocknum) + ".txt").c_str();
        fstream file;
        file.open(filename);
        file.seekg((line_size+1)*(num-1));
        getline(file, line);
        file.close();

        if(order == "desc")
                desc.push(make_pair(split_string(line, ' '), blocknum));
        else
                asc.push(make_pair(split_string(line, ' '), blocknum));
}

void mergeBlocks_asc(int num)
{
        vector<string> tmp;
        string line;
        for(int i=1;i<=num;i++)
        {
            add_line(i, ++c_line[i]);
        }
        cout << "Ascending Queue created" << endl;
        ofstream output;
        output.open(outfile);
        while(asc.empty()==false)
        {
                pair<vector<string>, int>p = asc.top();
                asc.pop();
                tmp = p.first;
                long long int n = tmp.size();
                for(int i=0; i<n; i++)
                {
                    output << tmp[i];
                    if(i != n-1)
                        output << " ";
                    else
                        output << endl;
                }
                c_line[p.second]++;
                add_line(p.second, c_line[p.second]);
        }
        output.close();
}

void mergeBlocks_desc(int num)
{
        vector<string> tmp;
        string line;
        for(int i=1;i<=num;i++)
        {
            add_line(i, ++c_line[i]);
        }
        cout << "Descending Queue created" << endl;
        ofstream output;
        output.open(outfile);
        while(desc.empty()==false)
        {
                pair<vector<string>, int>p = desc.top();
                desc.pop();
                tmp = p.first;
                long long int n = tmp.size();
                for(int i=0; i<n; i++)
                {
                    output << tmp[i];
                    if(i != n-1)
                        output << " ";
                    else
                        output << endl;
                }
                c_line[p.second]++;
                add_line(p.second, c_line[p.second]);
        }
        output.close();
}


bool compare(vector<string> a, vector<string> b)
{
    int num_cols = sort_order.size();
    for(int i=0; i<num_cols; i++)
    {
        if(order == "desc")
        {
            if(a[sort_order[i]] < b[sort_order[i]])
                    return 0;
            else if(a[sort_order[i]] > b[sort_order[i]])
                    return 1;
        }
        else
        {
            if(a[sort_order[i]] < b[sort_order[i]])
                    return 1;
            else if(a[sort_order[i]] > b[sort_order[i]])
                    return 0;
        }
    }
    return 0;
}

void merge(vector<vector<string>> arr, long long int l, long long int m, long long int r) 
{ 
    long long int i, j, k; 
    long long int n1 = m - l + 1; 
    long long int n2 =  r - m; 

    vector<vector<string>> L, R; 
  
    for (i = 0; i < n1; i++) 
        L.push_back(arr[l + i]); 
    for (j = 0; j < n2; j++) 
        R.push_back(arr[m + 1+ j]); 

    i = 0;  
    j = 0; 
    k = l; 
    while (i < n1 && j < n2) 
    { 
        cout<<compare(L[i],R[j]);
        if (compare(L[i],R[j])==0)
        
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++]; 
    } 
  
    while (i < n1) 
        arr[k++] = L[i++]; 

    while (j < n2) 
        arr[k++] = R[j++]; 
} 
void mergeSort(vector<vector<string>> arr, long long int l, long long int r) 
{ 
    if (l < r) 
    { 
        long long int m = (l+r)/2; 
        mergeSort(arr, l, m); 
        mergeSort(arr, m+1, r); 
  
        merge(arr, l, m, r); 
    } 
} 

void sortBlock(const char *name)
{
        ifstream block(name);
        string line;
        vector<vector<string>> bvec;
        while(getline(block, line))
            bvec.push_back(split_string(line, ' '));   
        block.close();  
        
        sort(bvec.begin(), bvec.end(),compare);

        ofstream block_out(name);
        long long int num_lines = bvec.size();
        for(long long int i=0; i<num_lines; i++)
        {
            int num_cols = bvec[i].size();
                for(int j=0; j<num_cols; j++)
                {
                    block_out << bvec[i][j];
                    if(j != (num_cols-1))
                        block_out << " ";
                    else
                        block_out << endl;
                }
        }
        block_out.close();
}

int main(int argc,char *argv[])
{       
    outfile = argv[2];
    int mem = stoi(argv[3]);
    order = argv[4];
    string line;
    map<string, int> colmap;
    int num_col = 0;
    ifstream md;
    md.open("metadata.txt");
    while(getline(md, line))
        colmap[split_string(line, ',')[0]] = num_col++;
    
    for(int i=5; i<argc ; i++)
        sort_order.push_back(colmap[argv[i]]);
    md.close();

    ifstream input(argv[1]);
    long long int lines = 0;
    while(getline(input, line))
    {
        if(lines==0)
            line_size = line.size();
        lines++;
    }
    input.close();
    long long int total_data = lines*line_size;
    long long int num_blocks = total_data/(mem*1024*1024); 
    if(num_blocks*mem*1024*1024 < total_data)
            num_blocks++;
    long long int block_lines = lines/num_blocks;
    cout << "Number of blocks: " << num_blocks <<endl;
    cout << "Number of lines per block: " << block_lines << endl;

    ifstream input1(argv[1]);
    for(int i=1; i<=num_blocks;i++)
    {
        string block_name = ("file" + to_string(i) + ".txt").c_str();
        ofstream blocks;
        blocks.open(block_name);
        for(long long int cnt=0 ; cnt < block_lines ; cnt++)
        { 
            getline(input1, line);
            blocks << line << endl;
            b_line[i]++;
        }
        blocks.close();
    }
    input1.close();
    
    for(int i=1;i<=num_blocks;i++)
    {
            string block_name = "file" + to_string(i) + ".txt";
            sortBlock(block_name.c_str());
    }
	if(order == "asc")	
    {
		mergeBlocks_asc(num_blocks);
	}
	else if(order == "desc")
	{
		mergeBlocks_desc(num_blocks);
	}
        return 0;
}
