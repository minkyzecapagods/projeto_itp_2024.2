# Código de Barras EAN-8

Este repositório contém dois programas para gerar e extrair códigos de barras do tipo EAN-8. Eles permitem gerar imagens de códigos de barras no formato PBM e extrair identificadores a partir dessas imagens.

## Descrição dos Programas

### 1. **Gerador de Código de Barras (gen)**

O programa **Gerador de Código de Barras** cria um arquivo de imagem PBM baseado no código de barras EAN-8 a partir de um identificador fornecido pelo usuário.

#### Como usar:

./gen <opção> ... <identificador>

#### Opções:

- `-m <pixels>`  
  Define a margem do código de barras, em pixels.  
  Sem a opção `-m`, a margem padrão será de **4px**.

- `-a <pixels>`  
  Define a área das barras, em pixels.  
  Sem a opção `-a`, a área das barras será de **3px**.

- `-h <pixels>`  
  Define a altura do código de barras, em pixels.  
  Sem a opção `-h`, a altura será de **50px**.

- `-n <file_name>`  
  Define o nome do arquivo de saída com a extensão `.pbm`.  
  Sem a opção `-n`, o nome do arquivo será o **identificador** fornecido.

#### Exemplo de uso:

./gen -m 6 -a 5 -h 60 -n codigo_barras.pbm 12345678

Isso gerará um arquivo **codigo_barras.pbm** com margem de **6px**, área das barras de **5px**, altura de **60px** e identificador **12345678**.

---

### 2. **Leitor de Código de Barras (ext)**

O programa **Leitor de Código de Barras** extrai o identificador do código de barras a partir de um arquivo de imagem PBM gerado pelo programa acima.

#### Como usar:

./ext <nome_do_arquivo>

#### Exemplo de uso:

./ext barcode-output/12345678.pbm

Isso irá extrair o identificador do código de barras do arquivo **12345678.pbm** localizado na pasta `barcode-output`.

### Observação:

- Para acessar arquivos dentro da pasta `barcode-output`, insira o caminho `barcode-output/` antes do nome do arquivo.

---

## Estrutura de Diretórios

my_project/
├── barcode-output/             # Contém os arquivos de saída .pbm gerados  
├── build/                      # Diretório de build  
│   ├── bin/                    # Executáveis (gen, ext)  
│   └── obj/                    # Arquivos objeto  
├── include/                    # Arquivos de cabeçalho  
├── Makefile                    # Script de build  
├── ext -> build/bin/ext         # Link simbólico para o executável ext  
├── gen -> build/bin/gen         # Link simbólico para o executável gen  
└── src/                        # Código-fonte (funções, io, etc.)  

## Compilação e Execução

Para compilar e executar os programas, basta usar o `Makefile`:

1. **Compilação**: Execute o comando `make` para compilar os programas.

   make

2. **Executar o Gerador de Código de Barras**:

   Após a compilação, você pode executar o gerador de código de barras com o comando:

   ./gen <opção> ... <identificador>

3. **Executar o Leitor de Código de Barras**:

   Para extrair o identificador do código de barras, use:

   ./ext <nome_do_arquivo>

## Licença

Este projeto está licenciado sob a Licença MIT. Veja o arquivo [LICENSE](LICENSE) para mais detalhes.
