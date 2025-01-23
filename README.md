# Gerador e Leitor de Código de Barras

Este repositório contém dois programas principais para a criação e leitura de códigos de barras:

- **Gerador de Código de Barras** (`gen`): Gera um arquivo `.pbm` com um código de barras baseado nas opções fornecidas.
- **Leitor de Código de Barras** (`ext`): Extrai o identificador de um código de barras a partir de um arquivo `.pbm`.

![Build Status](https://img.shields.io/badge/build-passing-brightgreen) ![License](https://img.shields.io/badge/license-MIT-blue) ![Version](https://img.shields.io/badge/version-1.0.0-orange)

## Requisitos

Antes de começar, verifique se você possui as seguintes ferramentas instaladas no seu sistema:

- **GCC** ou outro compilador C
- **Make** para automação do processo de compilação

Caso não tenha essas ferramentas instaladas, consulte a documentação oficial do GCC e do Make para instruções sobre como instalá-las.

## Clonando o Repositório

Para começar a usar este projeto, clone o repositório com o seguinte comando:

```bash
git clone https://github.com/lPitecus/projeto_itp.git
```

## Compilando o Projeto

O repositório já contém um `Makefile` que facilita o processo de compilação. Além disso, os arquivo compilados também já existem. No entanto, caso tenha mudado o código e deseja compilá-lo novamente, siga os passos abaixo para compilar os programas:

### Passo 1: Navegar até o diretório do projeto

Abra o terminal e navegue até o diretório onde você clonou o repositório:

```bash
cd projeto_itp
```

### Passo 2: Limpando os Arquivos Compilados

Use o `make clean` para limpar os arquivos já presente no repositório, como objetos e binários:

```bash
make clean
```

Isso irá remover todos os arquivos gerados na pasta `build`, incluindo os executáveis e arquivos objeto.

### Passo : Compilar o código

Use o `make` para compilar o projeto. O comando irá gerar os executáveis `gen` e `ext` dentro da pasta `build/bin`, além de seus atalhos no próprio diretório:

```bash
make
```

### Passo 3: Verificar a compilação

Após a compilação, os executáveis estarão localizados em:

```
gen             # Atalho para o executável
ext             # Atalho para o executável
build/bin/gen   # Gerador de Código de Barras
build/bin/ext   # Leitor de Código de Barras
```

## Como Usar o Gerador de Código de Barras (`gen`)

O programa **gen** permite ao usuário gerar códigos de barras em formato `.pbm` com diversas opções de personalização.

### Sintaxe Básica

Para executar o gerador de código de barras, utilize a seguinte sintaxe:

```bash
./gen <opção> ... <identificador>
```

### Opções de Linha de Comando

- `-m <pixels>`: Define a margem do código de barras em pixels.
  - Exemplo: `-m 10` define uma margem de 10px. Se não especificado, o valor padrão será 4px.
- `-a <pixels>`: Define a área das barras (largura) em pixels.
  - Exemplo: `-a 5` define a largura das barras como 5px. O valor padrão é 3px.
- `-h <pixels>`: Define a altura do código de barras em pixels.
  - Exemplo: `-h 70` define uma altura de 70px. O valor padrão é 50px.
- `-n <file_name>`: Define o nome do arquivo `.pbm` gerado.
  - Exemplo: `-n meu_codigo` gera um arquivo chamado `meu_codigo.pbm`. Se não especificado, o arquivo terá o mesmo nome do identificador.

### Exemplo de Uso

Para gerar um código de barras com:

- Margem de 5px
- Largura das barras de 4px
- Altura de 60px
- Nome do arquivo de saída como `codigo_barras`

Use o seguinte comando:

```bash
./gen -m 5 -a 4 -h 60 -n codigo_barras 12345678
```

Isso irá gerar um arquivo chamado `codigo_barras.pbm` no diretório `barcode-output` dentro do projeto com as configurações fornecidas.

## Como Usar o Leitor de Código de Barras (`ext`)

O programa **ext** permite que você extraia o identificador de um código de barras a partir de um arquivo `.pbm`.

### Sintaxe Básica

Para extrair o identificador de um código de barras, utilize o seguinte comando:

```bash
./ext <nome_do_arquivo>
```

### Exemplo de Uso

Para extrair o identificador de um código de barras a partir de um arquivo chamado `teste.pbm` localizado dentro da pasta `barcode-output/`, use o seguinte comando:

```bash
./ext barcode-output/teste.pbm
```

Isso irá exibir o identificador contido no código de barras do arquivo `teste.pbm`.

## Estrutura do Projeto

A estrutura do repositório é organizada da seguinte forma:

```bash
my_project/
├── barcode-output/             # Contém os arquivos de saída, como imagens .pbm
│   └── teste.pbm
├── include/                    # Contém os arquivos de cabeçalho (headers)
│   ├── definitions.h
│   ├── funcs.h
│   └── io.h
├── Makefile                    # Script de compilação
├── gen                         # Executável do Gerador de Código de Barras
├── ext                         # Executável do Leitor de Código de Barras
└── build/
    ├── bin/                    # Contém os executáveis compilados
    │   ├── gen
    │   └── ext
    └── obj/                    # Contém os arquivos objeto
        ├── EAN-8_code_generator.o
        ├── EAN-8_code_extractor.o
        ├── funcs.o
        ├── io.o
        └── definitions.o
```

### Descrição dos Diretórios

- **`barcode-output/`**: Pasta onde os arquivos gerados pelo programa `gen` (códigos de barras em `.pbm`) são armazenados.
- **`include/`**: Contém os arquivos de cabeçalho (`.h`) usados para declarar as funções e estruturas do projeto.
- **`build/`**: Contém a estrutura de arquivos compilados:
  - **`bin/`**: Onde os executáveis (`gen` e `ext`) são armazenados após a compilação.
  - **`obj/`**: Contém os arquivos objeto resultantes da compilação dos arquivos fonte.
- **`Makefile`**: Arquivo utilizado para automatizar o processo de compilação.

## Como Contribuir

Contribuições são bem-vindas! Siga as etapas abaixo para contribuir com o projeto:

1. **Fork o repositório**.
2. Crie uma **branch** para a sua feature ou correção (`git checkout -b minha-feature`).
3. Faça as modificações necessárias e **commit** suas mudanças (`git commit -am 'Adicionando nova feature'`).
4. Envie suas mudanças para o repositório forkado (`git push origin minha-feature`).
5. Abra um **Pull Request**.

## Licença

Este projeto está licenciado sob a Licença MIT. Consulte o arquivo [LICENSE](LICENSE) para mais detalhes.
