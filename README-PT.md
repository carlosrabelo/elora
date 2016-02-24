# elora

Engine 3D em C++14 na CPU — desenha num back buffer e só então apresenta numa janela X11.

## Destaques

- Inicia e encerra `elora::Engine`
- A aplicação informa o nome, o tamanho da janela e o tamanho do pixel (o demo incluso usa 3D Demo em 1024x768)
- O título da janela inclui a resolução, por exemplo `3D Demo 1024x768`
- Desenha num back buffer na CPU (`clear`, `put_pixel`, `fill_triangle`, `stroke_triangle`, `draw_text`); `present()` copia o buffer escalado para a janela X11
- `Engine::delta_time()` e `Engine::fps()` vêm do tempo real entre frames (sem sleep nem teto de FPS); o movimento da câmera e a rotação da malha usam `dt`
- `elora::update` monta o quadro no back buffer (malha com profundidade, iluminação Lambert e HUD); `present()` é a única cópia para a janela X11
- `elora::Vector3D` para posições e direções, com rotações em X, Y e Z
- `elora::Camera` com projeção perspectiva look-at (posição, alvo, FOV)
- `elora::Triangle` (indexado) e `elora::Mesh` (vértices e triângulos)
- Carrega e grava meshes Wavefront `.obj` (`assets/cube.obj`, plano, pirâmide, tetraedro, octaedro, bule)
- O demo incluso carrega `assets/teapot.obj` e gira numa cor só; mova a câmera com WASD (Z/X), R/F (Y) ou as setas; **T** alterna o wireframe; **C** alterna a paleta por face; **P** liga e desliga a auto-rotação; o FPS aparece no back buffer
- Compila com g++ (C++14) por meio de um fluxo Makefile
- Testes Catch2 para o ciclo de vida da engine e o desenho

## Pré-requisitos

- **g++ com C++14** — necessário para compilar a partir do código-fonte
- **libx11** — necessário para abrir a janela (`libx11-dev` no Debian/Ubuntu)
- **cppcheck** — necessário para `make lint`
- **clang-format** — necessário para `make fmt`

## Instalação

### Compilar a partir do código-fonte

```bash
git clone https://github.com/carlosrabelo/elora.git
cd elora
make setup
make build
```


## Uso

```bash
./bin/elora
```

Abre uma janela com o título `3D Demo 1024x768` e gira o bule carregado de `assets/teapot.obj`. Mova a câmera: **A/D** ou setas no X, **R/F** no Y, **W/S** ou setas no Z. **T** alterna o wireframe. **C** alterna cores por face (o padrão é uma cor só). **P** pausa ou retoma a auto-rotação. **Espaço** restaura a câmera e a rotação. Feche com Escape, `q` ou pelo gerenciador de janelas.

## Estrutura do Projeto

```
elora/               # componente C++ (fontes, testes, Makefile)
  src/core/          # Optional, Span
  src/engine/        # janela, back buffer, entrada
  src/math/          # Vector3D, Camera
  src/mesh/          # Triangle, Mesh, primitivas
  src/render/        # pipeline e rasterizador de malha
  src/io/            # Wavefront OBJ
assets/              # meshes Wavefront .obj
.make/               # Build and test scripts
bin/                 # Compiled binaries (git-ignored)
```

## Desenvolvimento

```bash
make setup           # Download Catch2 header (first time only)
make build           # Compile binary to bin/elora
make test            # Build and run tests
make quality         # Format, lint, and test
```

## Licença

Este projeto está licenciado sob a GNU General Public License v3.0 — veja [LICENSE](LICENSE) para detalhes.
