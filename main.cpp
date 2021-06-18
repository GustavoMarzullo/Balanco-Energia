#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define N 50 //numero máximo de correntes 
#define T_ref 25 //estado de referência

using namespace std;

typedef vector<double> vecdouble;
typedef vector<string> vecstring;

void clear(){//limpar tela
    /*
    Linux: system("clear")
    Windows: system("cls")
    */
    system("clear"); 
}

//############################################# Declarando variváveis globlais #######################################################

bool salvar=false; //variável a ser usada posteriormente para salvar os resultados

bool sistema_reativo=false; //variavel para guardar se o sistema é reativo ou não

string file_name;

unsigned numero_termos;

double entalpia_total_in, entalpia_total_out, delta;

vecdouble vazao_in(N), temperatura_in(N), vazao_out(N), temperatura_out(N); //vetores de dados de input
vecstring nomes(N); //vetor de nomes

vecdouble entalpia_especifica_in(N), entalpia_especifica_out(N),entalpia_in(N), entalpia_out(N), entalpia_formacao(N); //vetores de entalpia

vecdouble A(N), B(N); //termos A e B da tabela do Felder

//####################################################  FIM  ######################################################################

//############################################# Funções matemáticas ############################################################

vecdouble mult(const vecdouble& A,const vecdouble& B){//retorna um vetor C onde C[i]=A[i]*B[i]
    if(A.size()!=B.size()){
        throw("Tamanho de A diferente de B");
    }
    int ASize=A.size();
    vecdouble C(ASize);
    for(int i=0;i!=ASize;++i){
        C[i]=A[i]*B[i];
    }
    return C;
}

vecdouble soma(const vecdouble& A,const vecdouble& B){//retorna um vetor C onde C[i]=A[i]+B[i]
    if(A.size()!=B.size()){
        throw("Tamanho de A diferente de B");
    }
    int ASize=A.size();
    vecdouble C(ASize);
    for(int i=0;i!=ASize;++i){
        C[i]=A[i]+B[i];
    }
    return C;        
}

double func(double a, double b, double T){
    return (b*1e-5/2)*(T*T) + (a*1e-3)*T;
}

double integral(double T1, double T2, double a, double b){//integral de cp dt
    return func(a,b,T2)-func(a,b,T1);
}

double EntalpiaEspecifica(double a, double b, double T2){
    return integral(T_ref,T2,a,b);
}

double somatorio(const vecdouble& A){
    double temp=0;
    for(double i:A){
        temp+=i;
    }
    return temp;
}

//####################################################  FIM  ######################################################################

//############################################# Manipulação de arquivos ############################################################

bool ChecarExitencia(const string& myfile){//checa se myfile existe
    ifstream f(myfile.c_str());
    return f.good();
}

void CriarArquivo(string filename){//checa a existência do arquivo. se não existir cria um novo
    filename+=".txt";
    file_name=filename;
    if(not ChecarExitencia(filename)){
        fstream myfile;
        myfile.open(filename,ios::out); //create the file
        printf("\n");
        cout<<"Arquivo criado com sucesso!"<<endl;
        myfile.close();
    }
    else{
        printf("\n");
        cerr<<"Arquivo já existente. Escolha outro nome"<<endl;
    }
}

void BoasVindas(){
    cout<<"====== PROGRAMA PARA CALCULAR BALANÇO DE ENERGIA ======"<<endl;
    string input;
    cout<<"Deseja salvar um arquivo com os resultados?[S/N]"<<endl;
    getline(cin,input);
    printf("\n");
    if(input == "S" or input =="s"){
        string nome_arquivo;
        cout<<"Qual o nome do arquivo que deseja salvar?"<<endl;
        getline(cin,nome_arquivo);
        CriarArquivo(nome_arquivo);
        salvar=true;
    }

    cout<<"\nO sistema tem reação?[S/N]"<<endl;
    getline(cin,input);
    if(input == "S" or input =="s"){
        sistema_reativo=true;
    }

    cout<<"\nQuantos termos tem o seu balanço?"<<endl;
    cin>>numero_termos;
}

//####################################################  FIM  ######################################################################


//############################################# Função de input #################################################################

void InputTermos(){//input dos dados via console
    char Nomes[25];
    double Temperatura, Vazao;
    clear();
    cout<<"\nInsira a seguir:\n1)o nome de cada corrente de entrada;\n2)sua temperatura;\n3)vazão molar (se a vazão não existir na entrada mas na saída bote a vazão como zero)"<<endl;
    printf("\n");
    for(int i=0;i!=numero_termos;++i){
        cout<<"Corrente nº"<<i+1<<endl;
        scanf("%s %lf %lf",Nomes, &Temperatura, &Vazao);
        nomes[i]=(string)Nomes;
        temperatura_in[i]=Temperatura;
        vazao_in[i]=Vazao;
        printf("\n");
    }

    clear();
    cout<<"Insira na mesma ordem da entrada a temperatura e vazão de saída (se a vazão não existir na saída coloque vazão como zero)"<<endl;
    for(int i=0;i!=numero_termos;++i){
        cout<<"\nCorrente "<<nomes[i]<<endl;
        scanf("%lf %lf",&Temperatura, &Vazao);
        temperatura_out[i]=Temperatura;
        vazao_out[i]=Vazao;
    }

    if(sistema_reativo){
        clear();
        double entalpiaform;
        cout<<"Insira a entalpia de formação a 25ºC"<<endl;
        for(int i=0;i!=numero_termos;++i){
            cout<<"\nCorrente "<<nomes[i]<<endl;
            scanf("%lf",&entalpiaform);
            entalpia_formacao[i]=entalpiaform;
        }
    }

    clear();
    cout<<"Insira os termos a e b de cada corrente"<<endl;
    double a, b;
    for(int i=0;i!=numero_termos;++i){
        cout<<"\nCorrente "<<nomes[i]<<endl;
        scanf("%lf %lf",&a, &b);
        A[i]=a;
        B[i]=b;
    }
}
//####################################################  FIM  ######################################################################

//############################################# Calculo das entalpias #################################################################
void CalcEntalpiaEspecifica(){
    int i;
    double termoA, termoB, temp_in,temp_out,H_in,H_out;

    for(i=0;i!=numero_termos;++i){
        termoA=A[i];
        termoB=B[i];
        temp_in=temperatura_in[i];
        temp_out=temperatura_out[i];

        H_in=EntalpiaEspecifica(termoA, termoB, temp_in);
        H_out=EntalpiaEspecifica(termoA, termoB, temp_out);

        if(sistema_reativo){
            entalpia_especifica_in[i]=H_in+entalpia_formacao[i];
            entalpia_especifica_out[i]=H_out+entalpia_formacao[i];
        }
        else{
            entalpia_especifica_in[i]=H_in;
            entalpia_especifica_out[i]=H_out;
        }
    }
}

void CalcEntalpia(){
    CalcEntalpiaEspecifica();
    vecdouble in = mult(entalpia_especifica_in,vazao_in);
    vecdouble out = mult(entalpia_especifica_out, vazao_out);
    for(int i=0; i!=numero_termos;++i){
        entalpia_in[i]=in[i];
        entalpia_out[i]=out[i];
    }

    entalpia_total_in=somatorio(entalpia_in);
    entalpia_total_out=somatorio(entalpia_out);
    delta=entalpia_total_out-entalpia_total_in; 
}
//####################################################  FIM  ######################################################################

//########################################### Display dos resultados ###############################################################

void PrintResultados(){
    clear();
    printf("i\t   n_in\t\t    Ĥ_in\t   n_out\t   Ĥ_out\n");
    for(int i=0;i!=numero_termos;++i){
        cout<<nomes[i];
        printf("\t   %.4lf\t   %.4lf\t   %.4lf\t   %.4lf\n",vazao_in[i],entalpia_especifica_in[i],vazao_out[i], entalpia_especifica_out[i]);
    }
}

void PrintDelta(){
    printf("\nEntalpia Total de entrada = %.4lf\n",entalpia_total_in);
    printf("Entalpia total de saída = %.4lf\n",entalpia_total_out);
    printf("Variação de entalpia = %.4lf\n",delta);
}

void SalvarArquivo(){
    if(salvar){
        fstream myfile;
        myfile.open(file_name,ios::app);
        myfile<<"i\t   n_in\t    Ĥ_in\t   n_out\t   Ĥ_out\n";
        for(int i=0;i!=numero_termos;++i){
            myfile<<nomes[i]<<"\t   "<<vazao_in[i]<<"\t   "<<entalpia_especifica_in[i]<<"\t   "<<vazao_out[i]<<"\t   "<<entalpia_especifica_out[i]<<endl;
        }
        myfile<<"\nEntalpia Total de entrada = "<<entalpia_total_in<<endl;
        myfile<<"Entalpia total de saída = "<<entalpia_total_out<<endl;
        myfile<<"Variação de entalpia = "<<delta<<endl;
        myfile.close();
    }
}
//####################################################  FIM  ######################################################################

int main(){
    clear();
    BoasVindas();
    InputTermos();
    CalcEntalpia();
    PrintResultados();
    PrintDelta();
    SalvarArquivo();
    cout<<"\n\nFeito por Gustavo Marzullo"<<endl;
    //system("pause");
    return 0;
}