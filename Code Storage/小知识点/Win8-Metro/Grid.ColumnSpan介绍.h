//�µĶ�������Ҫѧϰ��
//��metro�Ĳ������й�grid�Ķ������治��
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

//Grid.ColumnSpan="2"�Ǹ������£���ʼ�㲻���⾿����ʲô��˼�����������ˣ�����ʾ���gridҪ��Խ��grid��
//2���У������ϵĴ�����Կ�������grid�������������У������grid�����κ�˵���Ļ����������ڸ�grid�ĵ�һ�е�һ�У�Ҳ����˵
//ֻ��Խһ�С����Ϊ������grid��Խ���л��߶��У��������Grid.ColumnSpan���ԡ�