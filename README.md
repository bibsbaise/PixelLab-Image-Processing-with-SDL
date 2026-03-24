# PixelLab: Image Processing with SDL

## Integrantes

1. Bianca Conceição Baise | RA: 10417489  
2. Daniel Yoshio Omiya | RA: 10356170  
3. Guilherme Vieira Rodrigues | RA: 10419633  
4. Júlia Petersen Aderne de Sá | RA: 10419605  

---

## Descrição do Projeto

Este projeto foi desenvolvido para a disciplina de Computação Visual (Professor André Kishimoto) da Universidade Presbiteriana Mackenzie.

O objetivo é implementar um software de processamento de imagens em C/C++, utilizando as bibliotecas SDL3, SDL_image e SDL_ttf.

O programa é executado via linha de comando, recebendo como entrada o caminho de uma imagem, e permite realizar análises e transformações na imagem, como conversão para escala de cinza, exibição de histograma e equalização.

---

## Funcionalidades

O sistema implementa as seguintes funcionalidades:

### 1. Carregamento de imagem
- Suporte aos formatos PNG, JPG e BMP
- Tratamento de erros (arquivo inexistente ou inválido)

---

### 2. Conversão para escala de cinza
- Verificação se a imagem já está em grayscale
- Conversão utilizando a fórmula:
  Y = 0.2125R + 0.7154G + 0.0721B


---

### 3. Exibição do histograma
- Cálculo do histograma da imagem
- Exibição gráfica na janela secundária
- Análise de:
  - Média da intensidade (imagem clara, média ou escura)
  - Desvio padrão (contraste alto, médio ou baixo)

---

### 4. Interface gráfica
- Janela principal: exibição da imagem
- Janela secundária: histograma + botão de interação

---

### 5. Equalização do histograma
- Equalização da imagem ao clicar no botão
- Atualização dinâmica da imagem e do histograma
- Alternância entre imagem original e equalizada

---

### 6. Salvamento da imagem
- Ao pressionar a tecla **S**, a imagem atual é salva como:
  output_image.png

- O arquivo é sobrescrito caso já exista

---

## Tecnologias Utilizadas

- Linguagem: C/C++
- SDL3
- SDL_image
- SDL_ttf

---

## Como compilar

### Linux / WSL:
```bash
g++ src/main.cpp -o programa -lSDL3 -lSDL3_image -lSDL3_ttf
```

### Windows (MinGW / MSYS2):
```bash
g++ src/main.cpp -o programa -lSDL3 -lSDL3_image -lSDL3_ttf.
```

OBS:
- Certifique-se de que as bibliotecas SDL3, SDL_image e SDL_ttf estão instaladas
- Os arquivos .dll devem estar na mesma pasta do executável ou no PATH do sistema

## Como executar
```bash
./programa caminho_da_imagem.ext
```

### Exemplo
```bash
./programa assets/imagem.png
```

## Estrutura do Projeto
proj1/
│
├── src/
│   └── main.cpp
├── include/
├── assets/
├── build/
├── README.md

## Responsabilidades dos Integrantes
Bianca — Carregamento de imagem, estrutura base e integração
Daniel — Interface gráfica e interação
Guilherme — Histograma e análise de imagem
Júlia — Conversão para escala de cinza

Cada integrante foi responsável por uma parte específica do projeto, conforme descrito acima. O desenvolvimento foi realizado de forma colaborativa, com integração contínua das funcionalidades.

## Observações Finais
- O projeto segue os requisitos definidos pelo enunciado da disciplina
- O código foi desenvolvido com foco em organização, modularização e clareza
