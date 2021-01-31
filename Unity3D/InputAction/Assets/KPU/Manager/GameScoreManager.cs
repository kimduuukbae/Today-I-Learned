namespace KPU.Manager
{
    public class GameScoreManager : SingletonBehaviour<GameScoreManager>
    {
        public int money;
        public int score;
        
        void Start()
        {
            EventManager.On("game_started", Initialize);
            EventManager.On("money_added", AddMoney);
            EventManager.On("score_added", AddScore);
            
        }

        private void AddMoney(object obj)
        {
            var addedMoney = (int)obj;
            money += addedMoney;
        }
        private void AddScore(object obj)
        {
            var addedScore = (int)obj;
            score += addedScore;
        }

        private void Initialize(object obj)
        {
            money = 10;
            score = 0;
        }
    }
}
