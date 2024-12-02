Projeto: Implementação de Operações SQL em C
Este projeto consiste na implementação de funcionalidades inspiradas em comandos SQL utilizando a linguagem C, com manipulação de arquivos binários para armazenamento eficiente dos dados. O trabalho explora conceitos avançados de sistemas de arquivos, incluindo índices secundários, árvores B e estratégias de alocação para registros removidos.

Funcionalidades Desenvolvidas
As funcionalidades foram projetadas para simular operações SQL em arquivos binários, abordando desde a criação de tabelas até buscas e atualizações otimizadas. Abaixo, uma descrição técnica das operações implementadas:

1. Criação de Tabela
Transfere os registros de um arquivo CSV para um arquivo binário estruturado. Essa operação organiza os dados de forma eficiente para futuras operações.

2. Seleção Simples
Lê e exibe todos os registros presentes no arquivo binário. Essa funcionalidade permite verificar o conteúdo do arquivo de maneira prática.

3. Seleção Condicional
Realiza buscas condicionais nos registros do arquivo binário com base em critérios definidos, simulando cláusulas WHERE do SQL para filtrar registros.

4. Criação de Índice Secundário
Gera um arquivo de índice secundário para otimizar buscas por campos específicos. O índice armazena pares chave-posições, permitindo localizar registros sem percorrer todo o arquivo binário.

5. Remoção Lógica e Física
Implementa a exclusão lógica de registros no arquivo binário, marcando-os como removidos, enquanto os registros correspondentes no índice secundário são removidos fisicamente. Essa abordagem evita reescritas desnecessárias no arquivo principal.

6. Inserção com Estratégia Best Fit
Insere novos registros no arquivo binário utilizando a estratégia best fit, que busca reutilizar o espaço de registros logicamente removidos. Essa abordagem minimiza o crescimento do arquivo e otimiza o uso de memória.

7. Criação de Índice com Árvore B
Constrói um arquivo de índice baseado em Árvore B, que oferece uma estrutura hierárquica para busca rápida por IDs. A Árvore B organiza os índices em nós balanceados, garantindo desempenho eficiente mesmo com grandes volumes de dados.

8. Busca com Índice de Árvore B
Permite localizar registros no arquivo binário utilizando um índice baseado em Árvore B. Essa operação otimiza buscas específicas por ID, evitando leituras desnecessárias do arquivo principal.

9. Busca Condicional com Índice de Árvore B
Realiza buscas no arquivo binário considerando critérios variados. Quando aplicável, utiliza o índice de Árvore B para melhorar o desempenho em buscas baseadas em ID.

10. Inserção com Índice de Árvore B
Insere novos registros no arquivo binário e atualiza o índice baseado em Árvore B para refletir as alterações. Essa funcionalidade combina eficiência de armazenamento com atualizações rápidas no índice.

Aspectos Técnicos do Projeto
Estrutura de Armazenamento
Arquivo Binário Principal: Contém os registros organizados em blocos. Cada registro possui metadados que indicam seu estado (ativo ou removido).
Arquivo de Índices Secundários: Gera tabelas de índice para otimizar operações de busca, permitindo localizar registros com base em chaves específicas.
Árvore B: Utilizada para busca e inserção eficiente. Sua estrutura balanceada mantém a complexidade das operações em 
𝑂
(
log
⁡
𝑛
)
O(logn).
Estratégias de Manipulação de Registros
Lista de Registros Removidos: Mantém um índice dos espaços disponíveis no arquivo principal, permitindo reutilização eficiente durante novas inserções.
Estratégia Best Fit: Utiliza o menor espaço disponível que possa acomodar o novo registro, otimizando o aproveitamento de memória.
Otimização e Eficiência
As operações que utilizam índices secundários ou árvores B minimizam leituras sequenciais do arquivo principal, reduzindo o tempo de execução.
A utilização de uma lista de registros removidos evita o crescimento desnecessário do arquivo binário e melhora a eficiência geral do sistema.
