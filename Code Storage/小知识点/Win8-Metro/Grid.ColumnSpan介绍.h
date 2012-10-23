//新的东西就是要学习啊
//在metro的布局中有关grid的东西还真不少
/*

<Grid Style="{StaticResource WindowsBlogLayoutRootStyle}">
    <Grid.RowDefinitions>
        <RowDefinition Height="140"/>
        <RowDefinition Height="*"/>
    </Grid.RowDefinitions>
    <Grid.ColumnDefinitions>
        <ColumnDefinition x:Name="primaryColumn" Width="610"/>
        <ColumnDefinition Width="*"/>
    </Grid.ColumnDefinitions>

    <!-- Back button and page title -->
    <Grid x:Name="titlePanel" Grid.ColumnSpan="2">
        <Grid.ColumnDefinitions>
            <ColumnDefinition Width="Auto"/>
            <ColumnDefinition Width="*"/>
        </Grid.ColumnDefinitions>
        <Button
            x:Name="backButton"
            Click="GoBack"
            IsEnabled="{Binding DefaultViewModel.CanGoBack, ElementName=pageRoot}"
            Style="{StaticResource BackButtonStyle}"/>
        <TextBlock x:Name="pageTitle" Grid.Column="1" Text="{Binding Feed.Title}" Style="{StaticResource PageHeaderTextStyle}">
            <TextBlock.Transitions>
                <TransitionCollection>
                    <ContentThemeTransition />
                </TransitionCollection>
            </TextBlock.Transitions>
        </TextBlock>
    </Grid>

*/

//Grid.ColumnSpan="2"是个新奇事，开始搞不懂这究竟是什么意思，现在明白了，它表示这个grid要跨越父grid的
//2个列，从以上的代码可以看出，父grid定义了两行两列，如果子grid不加任何说明的话，它将处在父grid的第一行第一列，也就是说
//只跨越一列。因此为了让子grid跨越两列或者多列，必须添加Grid.ColumnSpan属性。