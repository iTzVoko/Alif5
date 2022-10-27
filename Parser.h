#pragma once

// نتائج المحلل اللغوي
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum NodeType {
    NumberNode,
    StringNode,
    UnaryOpNode,
    BinOpNode,
    VarAccessNode,
    VarAssignNode,
    StatementCondationNode,
    ListNode,
    NameCallNode,
    NameCallArgsNode,
    BuildInFunctionNode, // تجربة فقط
    InverseNode,
    LogicNode,
    ExpressionsNode,

};

class Node {
public:
    Token token;
    NodeType type;
    Node* left;
    Node* right;
    std::list<Node*>* list_;

    Node(Node* left, Token token, Node* right, NodeType nodeType, std::list<Node*>* list_ = nullptr) {
        this->left = left;
        this->token = token;
        this->right = right;
        this->type = nodeType;
        this->list_ = list_;
    }

    ~Node() {
        //delete left;
        //delete right;
        //delete list_;
    }

};


// المحلل اللغوي
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Parser {
public:
    std::list<Token> tokens;
    int tokenIndex;
    Token currentToken;
    Node* node;
    Error* error;

    Parser(std::list<Token> tokens) : tokens(tokens)
    {
        tokenIndex = -1;
        this->advance();
    }

    ~Parser()
    {
        delete node;
        delete error;
    }

    void advance()
    {
        this->tokenIndex++;
        if (this->tokenIndex >= 0 and this->tokenIndex < this->tokens.size())
        {
            std::list<Token>::iterator listIter = tokens.begin();
            std::advance(listIter, this->tokenIndex);
            this->currentToken = *listIter;
        }
    }

    void reverse() {
        this->tokenIndex--;
        if (this->tokenIndex >= 0 and this->tokenIndex < this->tokens.size()) {
            std::list<Token>::iterator listIter = tokens.begin();
            std::advance(listIter, this->tokenIndex);
            this->currentToken = *listIter;
        }
    }

    void parse()
    {
        this->statements();
    }

    //////////////////////////////

    void atom() {
        Token token = this->currentToken;

        if (token.type_ == nameT)
        {
            this->advance();
            node = new Node(nullptr, token, nullptr, VarAccessNode);

        }
        else if (token.type_ == integerT or token.type_ == floatT)
        {
            this->advance();
            node = new Node(nullptr, token, nullptr, NumberNode); // قم بإنشاء صنف عقدة جديد ومرر فيه الرمز الذي تم حفظه في متغير رمز ومرر نوع العقدة المنشأءة واسندها الى متغير عقدة

        }
        else if (token.type_ == stringT) {
            this->advance();
            node = new Node(nullptr, token, nullptr, StringNode);

        }
        else if (token.type_ == keywordT and token.value_ == L"صح")
        {
            this->advance();
            node = new Node(nullptr, token, nullptr, StatementCondationNode);

        }
        else if (token.type_ == keywordT and token.value_ == L"خطا")
        {
            this->advance();
            node = new Node(nullptr, token, nullptr, StatementCondationNode);

        }
        else if (token.type_ == keywordT and token.value_ == L"عدم")
        {
            this->advance();
            node = new Node(nullptr, token, nullptr, StatementCondationNode);

        }
        else if (token.type_ == lSquareT)
        {
            this->advance();
            this->list_expr();
        }
    }

    void list_expr() 
    {
        Token token = this->currentToken;
        std::list<Node*>* nodeElement = new std::list<Node*>;

        if (this->currentToken.type_ == rSquareT)
        {
            this->advance();
        }
        else
        {
            this->expression(); // تقوم بتنفيذ التعبير وضبط نتيجة العملية في متغير node
            (*nodeElement).push_back(node);

            while (this->currentToken.type_ == commaT) {
                this->advance();
                this->expression();
                (*nodeElement).push_back(node);

            }

            if (this->currentToken.type_ != rSquareT)
            {
                error = new SyntaxError(this->currentToken.positionStart, this->currentToken.positionStart, L"لم يتم إغلاق قوس المصفوفة");
            }
            this->advance();
        }

        node = new Node(nullptr, Token(), nullptr, ListNode, nodeElement);

    }

    void primary() {
        Token name = this->currentToken;

        this->atom();

        if (name.type_ == nameT)
        {
            if (this->currentToken.type_ == lParenthesisT)
            {
                this->advance();
                if (this->currentToken.type_ != rParenthesisT)
                {
                    //this->arguments();
                    if (this->currentToken.type_ == rParenthesisT)
                    {
                        this->advance();
                    }
                }
                else if (this->currentToken.type_ == rParenthesisT)
                {
                    this->advance();
                    node = new Node(nullptr, name, node, BuildInFunctionNode);

                }
            }
            if (this->currentToken.type_ == dotT)
            {
                this->advance();
                this->primary();
                node = new Node(nullptr, name, node, NameCallNode);
            }

        }

        if (name.type_ == keywordT and name.value_ == L"اطبع")
        {
            this->advance();
            if (this->currentToken.type_ == lParenthesisT)
            {
                this->advance();
                if (this->currentToken.type_ != rParenthesisT)
                {
                    this->expression();
                    Node* expr = node;
                    node = new Node(nullptr, name, expr, BuildInFunctionNode);
                    if (this->currentToken.type_ == rParenthesisT)
                    {
                        this->advance();
                    }
                }
                else if (this->currentToken.type_ == rParenthesisT)
                {
                    this->advance();
                    node = new Node(nullptr, name, node, BuildInFunctionNode);

                }
            }
        }

    }

    void power()
    {
        bin_op_repeat(&Parser::primary, powerT, L" ", &Parser::factor);
    }

    void factor() {
        Token token = this->currentToken;
        Node* factor;

        if (token.type_ == plusT or token.type_ == minusT) {
            this->advance();
            this->factor();
            factor = node;
            node = new Node(nullptr, token, factor, UnaryOpNode);

        }

        this->power();
    }

    void term() {
        bin_op_repeat(&Parser::factor, multiplyT, divideT, &Parser::term);
    }

    void sum() {
        bin_op_repeat(&Parser::term, plusT, minusT, &Parser::sum);
    }

    void inversion() {
        Token token = this->currentToken;
        Node* expr;

        if (token.type_ == keywordT and token.value_ == L"ليس")
        {
            this->advance();
            this->sum();
            expr = node;
            node = new Node(nullptr, token, expr, InverseNode);

        }
        else {
            this->sum();

        }
    }

    void conjuction() {
        Token opToken;
        Node* left;
        Node* right;

        this->inversion();
        left = node;

        while (this->currentToken.type_ == keywordT and this->currentToken.value_ == L"و") {
            opToken = this->currentToken;
            opToken.type_ = L"And";
            this->advance();
            this->conjuction();

            right = node;

            left = new Node(left, opToken, right, LogicNode);
        }
        node = left;
    }

    void disjuction() {
        Token opToken;
        Node* left;
        Node* right;

        this->conjuction();
        left = node;

        while (this->currentToken.type_ == keywordT and this->currentToken.value_ == L"او") {
            opToken = this->currentToken;
            opToken.type_ = L"Or";
            this->advance();
            this->disjuction();

            right = node;

            left = new Node(left, opToken, right, LogicNode);
        }
        node = left;
    }

    void expression() {
        Token token = this->currentToken;
        Node* right;
        Node* left;

        this->disjuction();
        left = node;

        if (this->currentToken.type_ == keywordT and this->currentToken.value_ == L"اذا")
        {
            token = this->currentToken;
            this->advance();
            this->disjuction();
            right = node;
            node = new Node(left, token, right, BinOpNode); 
            if (this->currentToken.type_ == keywordT and this->currentToken.value_ == L"والا")
            {
                this->expression();
                right = node;
                node = new Node(left, this->currentToken, right, BinOpNode);
            }
        }
        else
        {
            node = left;

        }
    }

    void expressions() {
        Token opToken;
        Node* left;
        Node* right;

        this->expression();
        left = node;

        while (this->currentToken.type_ == commaT) {
            opToken = this->currentToken;
            this->advance();
            this->expression();

            right = node;

            left = new Node(left, opToken, right, ExpressionsNode);
        }
        node = left;
    }

    void class_defination() {
        expressions();
    }

    void function_defination() {
        class_defination();
    }

    void return_statement() {
        function_defination();
    }

    void while_statement() {
        return_statement();
    }

    void if_statement() {
        while_statement();
    }

    void import_from() {
        if_statement();
    }

    void import_name() {
        import_from();
    }

    void import_statement() {
        import_name();
    }

    void delete_statement() {
        import_statement();
    }

    void augassign() {
        delete_statement();
    }

    void assignment() {
        Token varName = this->currentToken;
        Node* expr;

        this->augassign();

        if (varName.type_ == nameT)
        {
            if (this->currentToken.type_ == equalT)
            {
                this->advance();
                this->assignment(); // نفذ المعادلة وضع القيم في node
                expr = node;
                node = new Node(nullptr, varName, expr, VarAssignNode);
            }
            //else {
            //    this->reverse();
            //}
        }
    }

    void compound_statement() {}

    void simple_statement()
    {
        assignment();
    }

    void statement() {
        simple_statement();
    }

    void statements() {
        statement();
    }



    void bin_op_repeat(void(Parser::* funcL)(), std::wstring fop, std::wstring sop, void(Parser::* funcR)()) {
        Token opToken;
        Node* left;
        Node* right;

        (this->*funcL)();
        left = node;

        while (this->currentToken.type_ == fop or this->currentToken.type_ == sop) {
            opToken = this->currentToken;
            this->advance();
            (this->*funcR)();

            right = node;

            left = new Node(left, opToken, right, BinOpNode);
        }
        node = left;
    }


    // طباعة نتائج المحلل اللغوي
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void print_node(Node* root, int space = 0, int t = 0) {

        if (error) {
            std::wcout << error->print_() << std::endl;
        }
        else {
            int count = 5;

            if (root == NULL)
                return;
            space += count;

            print_node(root->right, space, 1);

            for (int i = count; i < space; i++) {
                std::wcout << L" ";
            }

            if (t == 1) {
                std::wcout << L"/ " << root->token.type_ << L": " << root->token.value_ << std::endl;
            }
            else if (t == 2) {
                std::wcout << L"\\ " << root->token.type_ << L": " << root->token.value_ << std::endl;
            }
            else {
                //if (root->type == ListNode) { // لطباعة المصفوفة
                //    this->print_list(root);
                //}
                //else {
                std::wcout << root->token.type_ << std::endl;
                //}
            }
            print_node(root->left, space, 2);
        }
    }

    //void print_list(Node* root) {
    //    for (int i = 0; i < root->list_->size(); i++) {
    //        std::list<Node*> ::iterator listIter = root->list_->begin();
    //        std::advance(listIter, i);
    //        Node* a = *listIter;
    //        this->print_node(a);
    //    }
    //}

};