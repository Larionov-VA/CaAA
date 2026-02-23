#include <iostream>
#include <cstdint>
#include <algorithm>
#include <vector>

#include <climits>

#ifndef STEPIK
#define STEPIK 1
#endif

#if !STEPIK
#include <ctime>
#endif

struct SquareInfo {
    uint8_t xCoordinate;
    uint8_t yCoordinate;
    uint8_t sideSize;
};

struct LinkedList {
    LinkedList* next;
    SquareInfo result;
};

struct LinkColumn;
struct LinkNode;

struct LinkColumn {
    uint16_t columnId;
    uint16_t countOfIntersections;
    LinkColumn* leftColumn;
    LinkColumn* rightColumn;
    LinkNode* linkHead;

    LinkColumn(LinkColumn* prev, LinkColumn* head) {
        this->columnId = prev->columnId + 1;
        this->leftColumn = prev;
        prev->rightColumn = this;
        this->rightColumn = head;
        head->leftColumn = this;
        this->countOfIntersections = 0;
        this->linkHead = nullptr;
    }

    LinkColumn() {
        this->columnId = 0;
        this->leftColumn = this;
        this->rightColumn = this;
        this->countOfIntersections = 0;
        this->linkHead = nullptr;
    }
};

struct LinkNode {
    LinkNode* topNode;
    LinkNode* bottomNode;
    LinkNode* rigthNode;
    LinkNode* leftNode;
    LinkColumn* headColumn;
    SquareInfo row;
    bool wasFirst;

    LinkNode(LinkNode* leftNode, LinkColumn* headColumn, SquareInfo row) {
        this->row = row;
        this->headColumn = headColumn;
        this->wasFirst = false;

        if (leftNode) {
            this->leftNode = leftNode;
            this->rigthNode = leftNode->rigthNode;
            leftNode->rigthNode->leftNode = this;
            leftNode->rigthNode = this;
        } else {
            this->leftNode = this;
            this->rigthNode = this;
        }

        if (headColumn->linkHead) {
            this->topNode = headColumn->linkHead->topNode;
            this->bottomNode = headColumn->linkHead;
            headColumn->linkHead->topNode->bottomNode = this;
            headColumn->linkHead->topNode = this;
        } else {
            this->topNode = this;
            this->bottomNode = this;
            headColumn->linkHead = this;
        }
    }
};

struct IncidenceMatrix {
    LinkColumn* root;
    uint16_t columnCount;
};

uint8_t getMaxSquareSide(uint16_t currentPosition, uint8_t squareSide) {
    if (currentPosition) {
        uint8_t rigthDistance = squareSide - (currentPosition % squareSide);
        uint8_t bottomDistance = squareSide - (currentPosition / squareSide);
        return std::min(rigthDistance, bottomDistance);
    } else {
        return squareSide - 1;
    }
}

IncidenceMatrix buildMatrix(uint8_t squareSide) {
    uint16_t columnCount = squareSide * squareSide;
    IncidenceMatrix matrix;

    LinkColumn* root = new LinkColumn();
    matrix.root = root;
    matrix.columnCount = columnCount;

    LinkColumn** columnArray = new LinkColumn*[columnCount];

    LinkColumn* firstColumn = new LinkColumn(root, root);
    columnArray[0] = firstColumn;

    LinkColumn* prevColumn = firstColumn;
    for (uint16_t i = 1; i < columnCount; ++i) {
        LinkColumn* newColumn = new LinkColumn(prevColumn, root);
        columnArray[i] = newColumn;
        prevColumn = newColumn;
    }

    for (uint16_t pos = 0; pos < columnCount; ++pos) {
        uint8_t maxSide = getMaxSquareSide(pos, squareSide);
        for (uint8_t side = maxSide; side > 0; --side) {
            SquareInfo info = {
                (uint8_t)(pos % squareSide + 1),
                (uint8_t)(pos / squareSide + 1),
                side
            };
            LinkNode* left = nullptr;
            for (uint8_t dy = 0; dy < side; ++dy) {
                for (uint8_t dx = 0; dx < side; ++dx) {
                    uint16_t idx = pos + dy * squareSide + dx;
                    LinkColumn* col = columnArray[idx];
                    LinkNode* newNode = new LinkNode(left, col, info);
                    left = newNode;
                    col->countOfIntersections++;
                }
            }
        }
    }

    delete[] columnArray;
    return matrix;
}

void coverColumn(LinkColumn* col) {
    col->rightColumn->leftColumn = col->leftColumn;
    col->leftColumn->rightColumn = col->rightColumn;

    LinkNode* row = col->linkHead;
    if (!row) return;

    LinkNode* startRow = row;
    do {
        LinkNode* right = row->rigthNode;
        while (right != row) {
            right->topNode->bottomNode = right->bottomNode;
            right->bottomNode->topNode = right->topNode;

            if (right == right->headColumn->linkHead) {
                right->wasFirst = true;
                if (right->bottomNode == right) {
                    right->headColumn->linkHead = nullptr;
                } else {
                    right->headColumn->linkHead = right->bottomNode;
                }
            } else {
                right->wasFirst = false;
            }

            right->headColumn->countOfIntersections--;
            right = right->rigthNode;
        }
        row = row->bottomNode;
    } while (row != startRow);
}

void uncoverColumn(LinkColumn* col) {
    LinkNode* row = col->linkHead;
    if (row) {
        LinkNode* startRow = row;
        LinkNode* currentRow = row->topNode;
        do {
            LinkNode* left = currentRow->leftNode;
            while (left != currentRow) {
                left->topNode->bottomNode = left;
                left->bottomNode->topNode = left;

                if (left->wasFirst) {
                    left->headColumn->linkHead = left;
                    left->wasFirst = false;
                }

                left->headColumn->countOfIntersections++;
                left = left->leftNode;
            }
            currentRow = currentRow->topNode;
        } while (currentRow != startRow->topNode);
    }

    col->rightColumn->leftColumn = col;
    col->leftColumn->rightColumn = col;
}

LinkColumn* getCorrectColumn(IncidenceMatrix& matrix) {
    LinkColumn* root = matrix.root;
    LinkColumn* best = nullptr;
    uint16_t minCount = UINT16_MAX;

    LinkColumn* col = (LinkColumn*)root->rightColumn;
    while (col != root) {
        if (col->countOfIntersections < minCount) {
            minCount = col->countOfIntersections;
            best = col;
            if (minCount == 0) break;
        }
        col = (LinkColumn*)col->rightColumn;
    }
    return best;
}

static LinkedList* bestResult = nullptr;
static int bestCount = INT_MAX;

void cleanupResult(LinkedList& result) {
    LinkedList* current = result.next;
    while (current) {
        LinkedList* next = current->next;
        delete current;
        current = next;
    }
    result.next = nullptr;
}


int greedyUpperBound(int N) {
    std::vector<std::vector<bool>> used(N, std::vector<bool>(N, false));
    int count = 0;
    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x) {
            if (!used[y][x]) {
                int maxSide = 1;
                while (x + maxSide <= N && y + maxSide <= N) {
                    bool ok = true;
                    for (int dy = 0; dy < maxSide && ok; ++dy) {
                        for (int dx = 0; dx < maxSide && ok; ++dx) {
                            if (used[y + dy][x + dx]) ok = false;
                        }
                    }
                    if (ok) maxSide++;
                    else break;
                }
                maxSide--;
                if (N > 1 && x == 0 && y == 0 && maxSide == N) {
                    maxSide = N - 1;
                }
                for (int dy = 0; dy < maxSide; ++dy)
                    for (int dx = 0; dx < maxSide; ++dx)
                        used[y + dy][x + dx] = true;
                count++;
            }
        }
    }
    return count;
}


bool isPrime(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i * i <= n; i += 2)
        if (n % i == 0) return false;
    return true;
}


LinkColumn* findColumnById(LinkColumn* root, uint16_t id) {
    LinkColumn* col = (LinkColumn*)root->rightColumn;
    while (col != root) {
        if (col->columnId == id) return col;
        col = (LinkColumn*)col->rightColumn;
    }
    return nullptr;
}


bool applyKnownPatterns(IncidenceMatrix& matrix, LinkedList& result, int N) {
    if (N % 2 == 0) {
        int s = N / 2;
        SquareInfo squares[4] = {
            {1, 1, (uint8_t)s},
            {1, (uint8_t)(s + 1), (uint8_t)s},
            {(uint8_t)(s + 1), 1, (uint8_t)s},
            {(uint8_t)(s + 1), (uint8_t)(s + 1), (uint8_t)s}
        };
        for (int i = 0; i < 4; ++i) {
            uint16_t colId = (squares[i].yCoordinate - 1) * N + (squares[i].xCoordinate - 1) + 1;
            LinkColumn* col = findColumnById(matrix.root, colId);
            if (col) {
                LinkNode* node = col->linkHead;
                if (node) {
                    LinkNode* start = node;
                    do {
                        if (node->row.xCoordinate == squares[i].xCoordinate &&
                            node->row.yCoordinate == squares[i].yCoordinate &&
                            node->row.sideSize == squares[i].sideSize) {
                            LinkNode* curr = node;
                            do {
                                coverColumn(curr->headColumn);
                                curr = curr->rigthNode;
                            } while (curr != node);
                            LinkedList* newNode = new LinkedList;
                            newNode->result = node->row;
                            newNode->next = result.next;
                            result.next = newNode;
                            break;
                        }
                        node = node->bottomNode;
                    } while (node != start);
                }
            }
        }
        return true;
    }
    else if (N % 3 == 0) {
        int k = N / 3;
        int k2 = 2 * k;
        SquareInfo squares[6] = {
            {1, 1, (uint8_t)k2},
            {(uint8_t)(k2 + 1), 1, (uint8_t)k},
            {(uint8_t)(k2 + 1), (uint8_t)(k + 1), (uint8_t)k},
            {1, (uint8_t)(k2 + 1), (uint8_t)k},
            {(uint8_t)(k + 1), (uint8_t)(k2 + 1), (uint8_t)k},
            {(uint8_t)(k2 + 1), (uint8_t)(k2 + 1), (uint8_t)k}
        };
        for (int i = 0; i < 6; ++i) {
            uint16_t colId = (squares[i].yCoordinate - 1) * N + (squares[i].xCoordinate - 1) + 1;
            LinkColumn* col = findColumnById(matrix.root, colId);
            if (col) {
                LinkNode* node = col->linkHead;
                if (node) {
                    LinkNode* start = node;
                    do {
                        if (node->row.xCoordinate == squares[i].xCoordinate &&
                            node->row.yCoordinate == squares[i].yCoordinate &&
                            node->row.sideSize == squares[i].sideSize) {
                            LinkNode* curr = node;
                            do {
                                coverColumn(curr->headColumn);
                                curr = curr->rigthNode;
                            } while (curr != node);
                            LinkedList* newNode = new LinkedList;
                            newNode->result = node->row;
                            newNode->next = result.next;
                            result.next = newNode;
                            break;
                        }
                        node = node->bottomNode;
                    } while (node != start);
                }
            }
        }
        return true;
    }
    else if (isPrime(N)) {
        int s1 = N / 2 + 1;
        int s2 = N / 2;
        uint16_t colId1 = 1;
        LinkColumn* col1 = findColumnById(matrix.root, colId1);
        if (col1) {
            LinkNode* node = col1->linkHead;
            if (node) {
                LinkNode* start = node;
                do {
                    if (node->row.xCoordinate == 1 && node->row.yCoordinate == 1 && node->row.sideSize == s1) {
                        LinkNode* curr = node;
                        do {
                            coverColumn(curr->headColumn);
                            curr = curr->rigthNode;
                        } while (curr != node);
                        LinkedList* newNode = new LinkedList;
                        newNode->result = node->row;
                        newNode->next = result.next;
                        result.next = newNode;
                        break;
                    }
                    node = node->bottomNode;
                } while (node != start);
            }
        }
        int x2 = 1, y2 = s2 + 2;
        uint16_t colId2 = (y2 - 1) * N + (x2 - 1) + 1;
        LinkColumn* col2 = findColumnById(matrix.root, colId2);
        if (col2) {
            LinkNode* node = col2->linkHead;
            if (node) {
                LinkNode* start = node;
                do {
                    if (node->row.xCoordinate == x2 && node->row.yCoordinate == y2 && node->row.sideSize == s2) {
                        LinkNode* curr = node;
                        do {
                            coverColumn(curr->headColumn);
                            curr = curr->rigthNode;
                        } while (curr != node);
                        LinkedList* newNode = new LinkedList;
                        newNode->result = node->row;
                        newNode->next = result.next;
                        result.next = newNode;
                        break;
                    }
                    node = node->bottomNode;
                } while (node != start);
            }
        }
        int x3 = s2 + 2, y3 = 1;
        uint16_t colId3 = (y3 - 1) * N + (x3 - 1) + 1;
        LinkColumn* col3 = findColumnById(matrix.root, colId3);
        if (col3) {
            LinkNode* node = col3->linkHead;
            if (node) {
                LinkNode* start = node;
                do {
                    if (node->row.xCoordinate == x3 && node->row.yCoordinate == y3 && node->row.sideSize == s2) {
                        LinkNode* curr = node;
                        do {
                            coverColumn(curr->headColumn);
                            curr = curr->rigthNode;
                        } while (curr != node);
                        LinkedList* newNode = new LinkedList;
                        newNode->result = node->row;
                        newNode->next = result.next;
                        result.next = newNode;
                        break;
                    }
                    node = node->bottomNode;
                } while (node != start);
            }
        }
        return false;
    }
    return false;
}

void algorithmX(IncidenceMatrix& matrix, LinkedList& result, int depth) {
    if (depth >= bestCount) return;

    if (matrix.root->rightColumn == matrix.root) {
        if (depth <= bestCount) {
            cleanupResult(*bestResult);
            LinkedList* src = result.next;
            LinkedList* dstPrev = bestResult;
            while (src) {
                LinkedList* newNode = new LinkedList;
                newNode->result = src->result;
                newNode->next = nullptr;
                dstPrev->next = newNode;
                dstPrev = newNode;
                src = src->next;
            }
            bestCount = depth;
        }
        return;
    }

    LinkColumn* col = getCorrectColumn(matrix);
    if (!col) return;

    coverColumn(col);

    LinkNode* row = col->linkHead;
    if (!row) {
        uncoverColumn(col);
        return;
    }

    LinkNode* currentRow = row;
    do {
        if (depth + 1 >= bestCount) break;

        LinkedList* newNode = new LinkedList;
        newNode->result = currentRow->row;
        newNode->next = result.next;
        result.next = newNode;

        LinkNode* right = currentRow->rigthNode;
        while (right != currentRow) {
            coverColumn(right->headColumn);
            right = right->rigthNode;
        }

        algorithmX(matrix, result, depth + 1);

        LinkNode* left = currentRow->leftNode;
        while (left != currentRow) {
            uncoverColumn(left->headColumn);
            left = left->leftNode;
        }

        LinkedList* temp = result.next;
        result.next = temp->next;
        delete temp;

        currentRow = currentRow->bottomNode;

    } while (currentRow != row);

    uncoverColumn(col);
}

void cleanupMatrix(IncidenceMatrix& matrix) {
    LinkColumn* root = matrix.root;
    LinkColumn* current = (LinkColumn*)root->rightColumn;
    while (current != root) {
        LinkColumn* next = (LinkColumn*)current->rightColumn;

        LinkNode* node = current->linkHead;
        if (node) {
            LinkNode* startNode = node;
            do {
                LinkNode* nextNode = node->bottomNode;
                delete node;
                node = nextNode;
            } while (node != startNode);
        }

        delete current;
        current = next;
    }
    delete root;
}


int reduceToPrimeBase(int& N) {
    for (int p = 2; p * p <= N; ++p) {
        if (isPrime(p) && N % p == 0) {
            int scale = N / p;
            N = p;
            return scale;
        }
    }
    return 1;
}


int main() {
    int N;
    std::cin >> N;

#if !STEPIK
    clock_t start = clock();
#endif
    int scale = reduceToPrimeBase(N);
    IncidenceMatrix matrix = buildMatrix(N);
    LinkedList result;
    result.next = nullptr;
    bestResult = new LinkedList;
    bestResult->next = nullptr;
    bestCount = greedyUpperBound(N);
    bool complete = applyKnownPatterns(matrix, result, N);
    int initialDepth = 0;
    LinkedList* tmp = result.next;
    while (tmp) {
        initialDepth++;
        tmp = tmp->next;
    }
    if (complete) {
        cleanupResult(*bestResult);
        LinkedList* src = result.next;
        LinkedList* dstPrev = bestResult;
        while (src) {
            LinkedList* newNode = new LinkedList;
            newNode->result = src->result;
            newNode->next = nullptr;
            dstPrev->next = newNode;
            dstPrev = newNode;
            src = src->next;
        }
        bestCount = initialDepth;
    } else {
        algorithmX(matrix, result, initialDepth);
    }
    int count = 0;
    LinkedList* current = bestResult->next;
    while (current) {
        count++;
        current = current->next;
    }
    std::cout << count << '\n';
    current = bestResult->next;
    while (current) {
        int x = current->result.xCoordinate;
        int y = current->result.yCoordinate;
        int s = current->result.sideSize;

        if (scale > 1) {
            x = (x - 1) * scale + 1;
            y = (y - 1) * scale + 1;
            s = s * scale;
        }

        std::cout << x << ' ' << y << ' ' << s << '\n';
        current = current->next;
    }
#if !STEPIK
    clock_t end = clock();
    double seconds = (double)(end - start) / CLOCKS_PER_SEC;
    std::cout << "Время выполнения: " << seconds << " секунд" << std::endl;
#endif
    cleanupResult(result);
    cleanupResult(*bestResult);
    delete bestResult;
    cleanupMatrix(matrix);
    return 0;
}