#include <QString>
#include <QtTest>
#include <stdexcept>
#include <sstream>

class LogicError : public std::logic_error {
public:

    LogicError( int argument ) : std::logic_error( "" ), m_argument( argument ) {

    }

    virtual const char *what( ) const throw () {
        return m_message.c_str( );
    }

    virtual ~LogicError( ) throw () {

    }

protected:
    int m_argument;
    std::string m_message;
};

class InvalidArgument : public LogicError {
public:

    InvalidArgument( int argument ) : LogicError( argument ) {
        std::stringstream stream;
        stream << argument;
        m_message = "Argument " + stream.str( ) + " mush be multiple of 5";
    }
};

class OutOfRange : public LogicError {
public:

    OutOfRange( int argument, int beginOfRange, int endOfRange ) : LogicError( argument ) {
        std::string str_argument, str_beginOfRange, str_endOfRange;

        str_argument = intToString( argument );
        str_beginOfRange = intToString( beginOfRange );
        str_endOfRange = intToString( endOfRange );

        m_message = "Argument " + str_argument + " don't hit on the range [" +
                str_beginOfRange + ", " + str_endOfRange + "]";
    }

private:

    std::string intToString( int number ) {
        std::stringstream stream;
        stream << number;
        return stream.str( );
    }
};

double fiveAndFive( int number ) throw (InvalidArgument, OutOfRange);

class FiveAndFive : public QObject {
    Q_OBJECT

public:
    FiveAndFive( );

    static inline bool qFuzzyCompare( double p1, double p2, double delta ) {
        return (qAbs( p1 - p2 ) <= delta * qMin( qAbs( p1 ), qAbs( p2 ) ));
    }

    private
Q_SLOTS:
    void testCase1_data( );
    void testCase1( );
};

FiveAndFive::FiveAndFive( ) {
}

void FiveAndFive::testCase1_data( ) {
    QTest::addColumn<int>("number");
    QTest::addColumn<double>("expected");
    QTest::addColumn<bool>("isExpectedException");

    bool exception = true;
    bool no_exception = false;

    QTest::newRow( "fiveAndFive_01" ) << 0 << 0.0 << exception;
    QTest::newRow( "fiveAndFive_02" ) << 1 << 0.0 << exception;
    QTest::newRow( "fiveAndFive_03" ) << 4 << 0.0 << exception;
    QTest::newRow( "fiveAndFive_04" ) << 5 << 25.0 << no_exception;
    QTest::newRow( "fiveAndFive_05" ) << 6 << 0.0 << exception;
    QTest::newRow( "fiveAndFive_06" ) << 399995 << 159996000025.0 << no_exception;
    QTest::newRow( "fiveAndFive_07" ) << 400000 << 160000000000.0 << no_exception;
    QTest::newRow( "fiveAndFive_08" ) << 400001 << 0.0 << exception;
    QTest::newRow( "fiveAndFive_09" ) << 400005 << 0.0 << exception;
}

void FiveAndFive::testCase1( ) {
    QFETCH( int, number );
    QFETCH( double, expected );
    QFETCH( bool, isExpectedException );

    double delta = 0.0001;

    try {
        double actual = fiveAndFive( number );
        bool result = qFuzzyCompare( actual, expected, delta );
        if ( isExpectedException ) {
            QVERIFY2( false, "There is no exception." );
        } else {
            QString msg = QString( "\nActual: %1"
                    "\nExpected: %2"
                    "\nDelta: %3" ).arg( actual ).arg( expected ).arg( delta );
            QVERIFY2( result, msg.toStdString( ).c_str( ) );
        }

    } catch ( const LogicError& e ) {
        if ( !isExpectedException ) {
            QVERIFY2( false, "Exception was occur." );
        } else {
            QVERIFY2( true, "" );
        }
    } catch ( ... ) {
        QVERIFY2( false, "Uncaught exception." );
    }
}

QTEST_APPLESS_MAIN( FiveAndFive )

#include "tst_FiveAndFive.moc"
