//
// Created by ruiy on 18-3-16.
//

void test_ar(){
    //北京1987-2014人口: 35
    std::vector<double> test_data = {871.5,897.1,904.3,919.2,935.0,950.0,965.0,981.0,1028.0,1047.0,1061.0,1075.0,
                                     1086.0,1094.0,1102.0,1112.0,1125.0,1251.1,1259.4,1240.0,1245.6,1257.2,1363.6,
                                     1385.1,1423.2,1456.4,1492.7,1538.0,1601.0,1676.0,1771.0,1860.0,1961.9,2018.6,2069.3
    };
    // std::vector<double> test_data = {2,3,4,3,7};

    AR ar_model = AR(test_data);
    ar_model.fit("none_and_least_square");
    std::vector<double> res = ar_model.predict(37);

    ar_model.print_model_info();
}