#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QChar>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QKeyEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void btn_Num_Clicked();

private:
    void handleInput(const QString &input);
    void inputDigit(const QChar &digit);
    void inputDecimalPoint();
    void inputOperator(const QChar &op);
    void calculateResult();
    void applyUnaryOperation(const QString &operation);
    void backspace();
    void clearEntry();
    void clearAll();
    void showError(const QString &message);
    void updateExpression();
    QString displayOperator(const QChar &op) const;
    double currentValue() const;
    QString formatNumber(double value) const;
    bool calculate(double left, const QChar &op, double right, double &result) const;

    Ui::MainWindow *ui;
    double m_firstOperand = 0.0;
    double m_lastOperand = 0.0;
    QChar m_pendingOperator;
    QChar m_lastOperator;
    bool m_waitingForOperand = true;
    bool m_justEvaluated = false;
    bool m_hasError = false;
};
#endif // MAINWINDOW_H
