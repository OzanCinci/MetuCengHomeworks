package ceng.ceng351.foodrecdb;

import java.io.File;
import java.io.FileNotFoundException;
import java.sql.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;
import java.util.stream.StreamSupport;

public class FOODRECDB implements IFOODRECDB{

    private static final String user = "e2448223"; // TODO: Your userName
    private static final String password = "xwST-ABQlS2QjVzI"; //  TODO: Your password
    private static final String host = "momcorp.ceng.metu.edu.tr"; // host name
    private static final String database = "db2448223"; // TODO: Your database name
    private static final int port = 8080; // port

    private static Connection connection = null;

    @Override
    public void initialize() {
        String url = "jdbc:mysql://" + host + ":" + port + "/" + database;

        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            connection =  DriverManager.getConnection(url, user, password);
        }
        catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    @Override
    public int createTables() {
        int numberOfTablesCreated = 0;

        String createTableMenuItems = """
                CREATE TABLE IF NOT EXISTS MenuItems (
                itemID INT,
                itemName VARCHAR(40),
                cuisine VARCHAR(20),
                price INT,
                PRIMARY KEY(itemID)
                );
                """;

        String createTableIngredients = """
                CREATE TABLE IF NOT EXISTS Ingredients (
                ingredientID INT,
                ingredientName VARCHAR(40),
                PRIMARY KEY(ingredientID)
                );
                """;

        String createTableIncludes = """
                CREATE TABLE IF NOT EXISTS Includes (
                ingredientID INT,
                itemID INT,
                PRIMARY KEY (ingredientID,itemID),
                FOREIGN KEY (itemID) REFERENCES MenuItems(itemID),
                FOREIGN KEY (ingredientID) REFERENCES Ingredients(ingredientID)
                );
                """;

        String createTableRatings = """
                CREATE TABLE IF NOT EXISTS Ratings (
                ratingID INT,
                itemID INT,
                rating INT,
                ratingDate DATE,
                PRIMARY KEY(ratingID),
                FOREIGN KEY (itemID) REFERENCES MenuItems(itemID)
                );
                """;

        String createTableDietaryCategories = """
                CREATE TABLE IF NOT EXISTS DietaryCategories (
                ingredientID INT,
                dietaryCategory VARCHAR(20),
                PRIMARY KEY(ingredientID,dietaryCategory),
                FOREIGN KEY (ingredientID) REFERENCES Ingredients(ingredientID)
                );
                """;

       try {
           Statement stmt = connection.createStatement();

           stmt.execute(createTableMenuItems);
           numberOfTablesCreated++;

           stmt.execute(createTableIngredients);
           numberOfTablesCreated++;

           stmt.execute(createTableIncludes);
           numberOfTablesCreated++;

           stmt.execute(createTableRatings);
           numberOfTablesCreated++;

           stmt.execute(createTableDietaryCategories);
           numberOfTablesCreated++;

            stmt.close();
       } catch (SQLException e) {
           e.printStackTrace();
       }
        //System.out.println("create table done");
        return numberOfTablesCreated;
    }

    @Override
    public int dropTables() {
        int numberOfTableDropped = 0;
        String dropTableDietaryCategories = "DROP TABLE IF EXISTS DietaryCategories;";
        String dropTableRatings = "DROP TABLE IF EXISTS Ratings;";
        String dropTableIncludes = "DROP TABLE IF EXISTS Includes;";
        String dropTableIngredients = "DROP TABLE IF EXISTS Ingredients;";
        String dropTableMenuItems = "DROP TABLE IF EXISTS MenuItems;";

        try{
            Statement stmt = connection.createStatement();
            stmt.execute(dropTableDietaryCategories);
            numberOfTableDropped++;

            stmt.execute(dropTableRatings);
            numberOfTableDropped++;

            stmt.execute(dropTableIncludes);
            numberOfTableDropped++;

            stmt.execute(dropTableIngredients);
            numberOfTableDropped++;

            stmt.execute(dropTableMenuItems);
            numberOfTableDropped++;

            stmt.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        //System.out.println("drop table done");
        return numberOfTableDropped;
    }

    @Override
    public int insertMenuItems(MenuItem[] items) {
        String insertQuery;
        int numberOfRows = 0;
        try {
            Statement stmt = connection.createStatement();
            for (MenuItem item : items){
                insertQuery = String.format("INSERT INTO MenuItems VALUES( %d , '%s' , '%s' , %d )",item.getItemID(),item.getItemName(),item.getCuisine(),item.getPrice());
                stmt.executeUpdate(insertQuery);
                numberOfRows++;
            }
            stmt.close();
        }  catch (SQLException e) {
            e.printStackTrace();
        }
        //System.out.println("insert menu items done");
        return numberOfRows;
    }

    @Override
    public int insertIngredients(Ingredient[] ingredients) {
        String insertQuery;
        int numberOfRows = 0;
        try {
            Statement stmt = connection.createStatement();
            for (Ingredient ingredient : ingredients){
                insertQuery = String.format("INSERT INTO Ingredients VALUES( %d , '%s' )", ingredient.getIngredientID(),ingredient.getIngredientName());
                stmt.executeUpdate(insertQuery);
                numberOfRows++;
            }
            stmt.close();
        }  catch (SQLException e) {
            e.printStackTrace();
        }
        //System.out.println("insert ingredients done");
        return numberOfRows;
    }

    @Override
    public int insertIncludes(Includes[] includes) {
        String insertQuery;
        int numberOfRows = 0;
        try {
            Statement stmt = connection.createStatement();
            for (Includes include : includes){
                insertQuery = String.format("INSERT INTO Includes VALUES( %d , %d )", include.getIngredientID(),include.getItemID());
                stmt.executeUpdate(insertQuery);
                numberOfRows++;
            }
            stmt.close();
        }  catch (SQLException e) {
            e.printStackTrace();
        }
        //System.out.println("insert includes done");
        return numberOfRows;
    }

    @Override
    public int insertDietaryCategories(DietaryCategory[] categories) {
        String insertQuery;
        int numberOfRows = 0;
        try {
            Statement stmt = connection.createStatement();
            for (DietaryCategory category : categories){
                insertQuery = String.format("INSERT INTO DietaryCategories VALUES( %d , '%s' )", category.getIngredientID(),category.getDietaryCategory());
                stmt.executeUpdate(insertQuery);
                numberOfRows++;
            }
            stmt.close();
        }  catch (SQLException e) {
            e.printStackTrace();
        }
        //System.out.println("insert dietary done");
        return numberOfRows;
    }

    @Override
    public int insertRatings(Rating[] ratings) {
        String insertQuery;
        int numberOfRows = 0;
        try {
            Statement stmt = connection.createStatement();
            for (Rating rating : ratings){
                insertQuery = String.format("INSERT INTO Ratings VALUES( %d , %s , %s , '%s' )", rating.getRatingID(),rating.getItemID(),rating.getRating(),rating.getRatingDate());
                stmt.executeUpdate(insertQuery);
                numberOfRows++;
            }
            stmt.close();
        }  catch (SQLException e) {
            e.printStackTrace();
        }
        //System.out.println("insert ratings done");
        return numberOfRows;
    }

    //DONE
    @Override
    public MenuItem[] getMenuItemsWithGivenIngredient(String name) {
        //System.out.println("starting get menu with given ingredients");
        ArrayList<MenuItem> temp = new ArrayList<>();
        String query = String.format("""
                SELECT M.*
                FROM
                    (SELECT I.ingredientID
                    FROM Ingredients I
                    WHERE I.ingredientName='%s') AS Temp,
                    MenuItems M, Includes I
                WHERE 1=1
                    and I.ingredientID = Temp.ingredientID
                    and I.itemId=M.ItemId
                ORDER BY itemID ASC
                """, name);
        try {
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery(query);

            while (rs.next()) {
                int itemID = rs.getInt(1);
                String itemName = rs.getString(2);
                String cuisine = rs.getString(3);
                int price = rs.getInt(4);
                MenuItem tempMenuItem = new MenuItem(itemID, itemName, cuisine, price);
                temp.add(tempMenuItem);
                //System.out.println(tempMenuItem);
            }
            MenuItem[] result = new MenuItem[temp.size()];
            for (int idx = 0; idx < temp.size(); idx++)
                result[idx] = temp.get(idx);

            stmt.close();
            return result;
            }  catch(SQLException e){
                e.printStackTrace();
            }

        return new MenuItem[0];
    }

    //DONE
    @Override
    public MenuItem[] getMenuItemsWithoutAnyIngredient() {
        String query = """
                SELECT M.*
                FROM MenuItems M
                WHERE NOT EXISTS (
                                    SELECT *
                                    FROM Includes I
                                    WHERE M.itemId=I.itemId)
                ORDER BY itemID ASC""";
        //System.out.println("starting getMenuItemsWithoutAnyIngredient");
        ArrayList<MenuItem> temp = new ArrayList<>();
        try{
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery(query);

            while (rs.next()) {
                int itemID = rs.getInt(1);
                String itemName = rs.getString(2);
                String cuisine = rs.getString(3);
                int price = rs.getInt(4);
                MenuItem tempMenuItem = new MenuItem(itemID, itemName, cuisine, price);
                temp.add(tempMenuItem);
                //System.out.println(tempMenuItem);
            }

            MenuItem[] result = new MenuItem[temp.size()];
            for (int idx = 0; idx < temp.size(); idx++)
                result[idx] = temp.get(idx);

            stmt.close();
            return result;

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return new MenuItem[0];
    }

    //DONE
    @Override
    public Ingredient[] getNotIncludedIngredients() {
        String query = """
                SELECT I.*
                FROM Ingredients I
                WHERE I.ingredientID NOT IN (
                        SELECT DISTINCT I2.ingredientID
                        FROM Includes I2
                    )
                ORDER BY ingredientID ASC
                """;
        ArrayList<Ingredient> temp = new ArrayList<>();
        //System.out.println("starting getNotIncludedIngredients");

        try{
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery(query);

            while(rs.next()){
                int ingredientID = rs.getInt(1);
                String ingredientName = rs.getString(2);
                Ingredient tempIngredient = new Ingredient(ingredientID,ingredientName);
                temp.add(tempIngredient);
                //System.out.println(tempIngredient);
            }

            Ingredient[] result = new Ingredient[temp.size()];
            int idx=0;
            for(Ingredient ingredient: temp){
                result[idx++]=ingredient;
            }

            stmt.close();
            return result;
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return new Ingredient[0];
    }

    //DONE -> check distinct condition
    @Override
    public MenuItem getMenuItemWithMostIngredients() {
        String query = """
                SELECT M.*
                FROM
                       (SELECT I.itemID
                        FROM Includes I
                        GROUP BY I.itemID
                        HAVING NOT EXISTS (
                                    SELECT I2.itemID
                                    FROM Includes I2
                                    GROUP BY I2.itemID
                                    HAVING COUNT(I2.ingredientID) > COUNT(I.ingredientID)
                                )) AS IDs, MenuItems M
                WHERE M.itemID=IDs.itemID
                """;
        //System.out.println("starting getMenuItemWithMostIngredients");

        try{
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery(query);

            rs.next();
            int itemID = rs.getInt(1);
            String itemName = rs.getString(2);
            String cuisine = rs.getString(3);
            int price = rs.getInt(4);
            MenuItem tempMenuItem = new MenuItem(itemID, itemName, cuisine, price);
            //System.out.println(tempMenuItem);

            stmt.close();
            return tempMenuItem;
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return null;
    }

    //DONE
    @Override
    public QueryResult.MenuItemAverageRatingResult[] getMenuItemsWithAvgRatings() {
        ArrayList<QueryResult.MenuItemAverageRatingResult> temp = new ArrayList<>();
        String query = """
                SELECT M.itemID, M.itemName, AVG(R.rating) as avgRating
                FROM  MenuItems M NATURAL LEFT OUTER JOIN Ratings R
                GROUP BY M.itemID
                ORDER BY avgRating DESC;
                """;

        //System.out.println("starting getMenuItemsWithAvgRatings");

        try{
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery(query);

            while(rs.next()){
                QueryResult.MenuItemAverageRatingResult tempMenuItemAverageRatingResult = new QueryResult.MenuItemAverageRatingResult(rs.getString(1),rs.getString(2),rs.getString(3));
                //System.out.println(tempMenuItemAverageRatingResult);
                temp.add(tempMenuItemAverageRatingResult);
            }

            QueryResult.MenuItemAverageRatingResult[] result = new QueryResult.MenuItemAverageRatingResult[temp.size()];
            int idx = 0;
            for(QueryResult.MenuItemAverageRatingResult el: temp)
                result[idx++] = el;

            stmt.close();
            return result;
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return new QueryResult.MenuItemAverageRatingResult[0];
    }

    //DONE
    @Override
    public MenuItem[] getMenuItemsForDietaryCategory(String category) {
        ArrayList<MenuItem> temp = new ArrayList<>();
        String query = String.format("""
                SELECT DISTINCT M.*
                FROM
                   (SELECT DISTINCT I.itemID
                    FROM Includes I
                    WHERE NOT EXISTS (
                            SELECT DISTINCT I2.ingredientID
                            FROM Includes I2
                            WHERE 1=1
                      			and I2.itemID = I.itemID
                				and I2.ingredientID NOT IN (
                                  SELECT DISTINCT I3.ingredientID
                                  FROM Includes I3, DietaryCategories D
                                  WHERE 1=1
                                      and I3.ingredientID = D.ingredientID
                                      and D.dietaryCategory = '%s'
                                )
                        )) AS ItemIDs, MenuItems M
                WHERE ItemIDs.itemID=M.itemID
                ORDER BY M.itemID ASC
                """,category);
        //System.out.println("starting getMenuItemsForDietaryCategory");

        try{
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery(query);

            while(rs.next()){
                int itemID = rs.getInt(1);
                String itemName = rs.getString(2);
                String cuisine = rs.getString(3);
                int price = rs.getInt(4);
                MenuItem tempMenuItem = new MenuItem(itemID,itemName,cuisine,price);
                //System.out.println(tempMenuItem);
                temp.add(tempMenuItem);
            }
            MenuItem[] result = new MenuItem[temp.size()];
            int idx = 0;
            for(MenuItem menuItem: temp)
                result[idx] = menuItem;

            stmt.close();
            return result;

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return new MenuItem[0];
    }

    //DONE
    @Override
    public Ingredient getMostUsedIngredient() {
        String query = """
                SELECT I.*
                FROM
                       (SELECT I.ingredientID
                        FROM Includes I
                        GROUP BY I.ingredientID
                        HAVING NOT EXISTS (
                                SELECT I.ingredientId
                                FROM Includes I2
                                GROUP BY I2.ingredientID
                                HAVING COUNT(I2.itemID)> COUNT(I.itemID)
                            )) AS Temp1, Ingredients I
                WHERE I.ingredientID = Temp1.ingredientID
                """;
        //System.out.println("starting getMostUsedIngredient");

        try{
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery(query);
            rs.next();
            int ingredientID = rs.getInt(1);
            String ingredientName = rs.getString(2);
            Ingredient tempIngredient = new Ingredient(ingredientID,ingredientName);
            //System.out.println(tempIngredient);

            stmt.close();
            return tempIngredient;

        } catch (SQLException e) {
            e.printStackTrace();
        }

        return null;
    }

    //DONE
    @Override
    public QueryResult.CuisineWithAverageResult[] getCuisinesWithAvgRating() {
        ArrayList<QueryResult.CuisineWithAverageResult> temp = new ArrayList<>();
        String query = """
                SELECT M.cuisine, AVG(R.rating) AS averageRating
                FROM MenuItems M NATURAL LEFT OUTER JOIN Ratings R
                GROUP BY M.cuisine
                ORDER BY 2 DESC
                """;
        //System.out.println("starting getCuisinesWithAvgRating");

        try{
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery(query);

            while(rs.next()){
                String cuisineName = rs.getString(1);
                String average = rs.getString(2);
                QueryResult.CuisineWithAverageResult tempCuisineWithAverageResult = new QueryResult.CuisineWithAverageResult(cuisineName,average);
                temp.add(tempCuisineWithAverageResult);
                //System.out.println(tempCuisineWithAverageResult);
            }

            QueryResult.CuisineWithAverageResult[] result = new QueryResult.CuisineWithAverageResult[temp.size()];
            int idx=0;
            for(QueryResult.CuisineWithAverageResult el: temp)
                result[idx++] = el;

            stmt.close();
            return result;
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return new QueryResult.CuisineWithAverageResult[0];
    }

    //DONE
    @Override
    public QueryResult.CuisineWithAverageResult[] getCuisinesWithAvgIngredientCount() {
        ArrayList<QueryResult.CuisineWithAverageResult> temp = new ArrayList<>();
        String query = """
                SELECT Temp.cuisine, AVG(Temp.IngredientCount) as average
                FROM
                    (
                    SELECT M.itemID, M.itemName, M.cuisine, M.price, COUNT(I.ingredientID) as IngredientCount
                    FROM MenuItems M NATURAL LEFT OUTER JOIN Includes I
                    GROUP BY itemID
                    ) AS Temp
                GROUP BY cuisine
                ORDER BY average DESC;
                """;
        //System.out.println("starting getCuisinesWithAvgIngredientCount");
        try{
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery(query);

            while(rs.next()){
                String cuisineName = rs.getString(1);
                String average = rs.getString(2);
                QueryResult.CuisineWithAverageResult tempCuisineWithAverageResult = new QueryResult.CuisineWithAverageResult(cuisineName,average);
                temp.add(tempCuisineWithAverageResult);
                //System.out.println(tempCuisineWithAverageResult);
            }

            QueryResult.CuisineWithAverageResult[] result = new QueryResult.CuisineWithAverageResult[temp.size()];
            int idx=0;
            for(QueryResult.CuisineWithAverageResult el: temp)
                result[idx++] = el;

            stmt.close();
            return result;
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return new QueryResult.CuisineWithAverageResult[0];
    }

    //DONE
    @Override
    public int increasePrice(String ingredientName, String increaseAmount) {
        int numberOfRowsAffected = 0;
        String query = String.format("""
                UPDATE MenuItems
                SET price = price + %s
                WHERE MenuItems.itemID IN
                       (SELECT Inc.itemID
                        FROM Ingredients I, Includes Inc
                        WHERE 1=1
                            and I.ingredientID = Inc.ingredientID
                            and I.ingredientName = '%s'
                        )
                """,increaseAmount,ingredientName);
        //System.out.println("starting increasePrice");
        try{
            Statement stmt = connection.createStatement();
            numberOfRowsAffected = stmt.executeUpdate(query);
            //System.out.println(numberOfRowsAffected);
            stmt.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        return numberOfRowsAffected;
    }

    //DONE
    @Override
    public Rating[] deleteOlderRatings(String date) {

        ArrayList<Rating> temp = new ArrayList<>();
        String querySelect = String.format(
                """
                    SELECT *
                    FROM Ratings R
                    WHERE R.ratingDate<'%s'
                """
        ,date);
        String queryDelete = String.format(
                """
                    DELETE FROM Ratings R
                    WHERE R.ratingDate<'%s'
                """
                ,date);
        //System.out.println("starting deleteOlderRatings");
        try{
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery(querySelect);
            while(rs.next()){
                Rating tempRating = new Rating(rs.getInt(1),rs.getInt(2),rs.getInt(3),rs.getString(4));
                //System.out.println(tempRating);
                temp.add(tempRating);
            }
            Rating[] result = new Rating[temp.size()];
            int idx = 0;
            for(Rating rating: temp)
                result[idx++] = rating;

            stmt.execute(queryDelete);

            stmt.close();
            return result;
        } catch (SQLException e) {
            //System.out.println("CASE: deleteOlderRatings: " + e.getMessage() + " " + e.getSQLState());
        }

        return new Rating[0];
    }
}
