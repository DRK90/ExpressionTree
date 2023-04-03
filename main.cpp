#include "classes.hpp"

double evaluate_expression_tree(std::shared_ptr<ExprNode> node);

using node_ptr = std::shared_ptr<ExprNode>;

int main()
{
    
    VariableNode::set_value("Xray", 1.0);
    VariableNode::set_value("Yellow", 2.0);
    VariableNode::set_value("Zebra", 3.0);

    node_ptr node = std::make_shared<AddNode>(
        std::make_shared<MulNode>(
            std::make_shared<ConstantNode>(2.3),
            std::make_shared<VariableNode>("Xray")),
        std::make_shared<MulNode>(
            std::make_shared<VariableNode>("Yellow"),
            std::make_shared<SubNode>(
                std::make_shared<VariableNode>("Zebra"),
                std::make_shared<VariableNode>("Xray"))));
                

    double result = evaluate_expression_tree(node);

    std::cout <<"\n"<< *node;
    std::cout << "\n Evaluates to: " << result << "\n\n";

    node_ptr dx = node->derivative("Xray");
    double derivativeResult = evaluate_expression_tree(dx);
    std::cout << *dx;
    std::cout << "\nEvaluates to: " << derivativeResult << "\n\n";

    

    return 0;
}



double evaluate_expression_tree(std::shared_ptr<ExprNode> node)
    {
        return node->eval();
    };